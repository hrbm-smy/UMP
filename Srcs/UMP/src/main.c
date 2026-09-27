#include <string.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/util/queue.h"
#include "hardware/uart.h"
#include "pio_usb.h"
#include "tusb.h"
#include "ArrayCap.h"
#include "HidMouseDescriptorMap.h"

#define LED_PIN           25
#define BLINK_INTERVAL_MS 1000
#define USB_DP_PIN        0   // D+ = GPIO 0, D- = GPIO 1 (自動)
#define DEBUG_UART_ID     uart0
#define DEBUG_UART_BAUD   115200
#define DEBUG_UART_TX_PIN 16
#define BTN_MASK_OUT      0x07
#define BTN_X1_MASK       0x08
#define BTN_NON_X1_MASK   ((uint8_t)~BTN_X1_MASK)
#define TRACK_SCROLL_THRESHOLD 16

#define REPORT_QUEUE_SIZE 16
#define REPORT_MAX_LEN    16

typedef struct {
    uint8_t data[REPORT_MAX_LEN];
    uint8_t len;
} hid_report_t;

static queue_t s_report_queue;
static bool s_track_scroll_mode = false;
static uint8_t s_prev_buttons = 0;
static int16_t s_scroll_acc_v = 0;
static int16_t s_scroll_acc_h = 0;

static void debug_uart_init(void)
{
    uart_init(DEBUG_UART_ID, DEBUG_UART_BAUD);
    gpio_set_function(DEBUG_UART_TX_PIN, GPIO_FUNC_UART);
}

static void debug_uart_puts(char const *s)
{
    uart_puts(DEBUG_UART_ID, s);
}

static int8_t clamp_i16_to_i8(int16_t v)
{
    if (v > 127) return 127;
    if (v < -127) return -127;
    return (int8_t)v;
}

static int8_t consume_scroll_steps(int16_t *acc, int16_t delta)
{
    *acc += delta;

    int16_t steps = 0;
    while (*acc >= TRACK_SCROLL_THRESHOLD) {
        *acc -= TRACK_SCROLL_THRESHOLD;
        steps++;
    }
    while (*acc <= -TRACK_SCROLL_THRESHOLD) {
        *acc += TRACK_SCROLL_THRESHOLD;
        steps--;
    }

    return clamp_i16_to_i8(steps);
}

// 受信レポートから出力へのマッピング情報
// dev_addr は 1-based で最大 CFG_TUH_DEVICE_MAX
#define DEVICE_MAP_MAX (CFG_TUH_DEVICE_MAX + 1)
static HidMouseDescriptorMap s_device_map[DEVICE_MAP_MAX];

// ---- HID Host コールバック (Core1 から呼ばれる) ----

void tuh_hid_report_received_cb(
    uint8_t dev_addr, uint8_t instance,
    uint8_t const *report, uint16_t len)
{
    const HidMouseDescriptorMap *map =
        &s_device_map[dev_addr < DEVICE_MAP_MAX ? dev_addr : 0];

    // Report ID がある場合は先頭1バイトをスキップ
    if (map->ReportId != 0 && len > 0) {
        report++;
        len--;
    }

    hid_report_t r;
    r.len = 5;
    // デスクリプタ解析結果に基づいてバイトを取り出す
    uint8_t src_btn = (uint8_t)HidMouseDescriptorMap_Buttons(report, len, map);
    int8_t src_x = (int8_t)HidMouseDescriptorMap_X(report, len, map);
    int8_t src_y = (int8_t)HidMouseDescriptorMap_Y(report, len, map);
    int8_t src_wheel = (int8_t)HidMouseDescriptorMap_VWheel(report, len, map);
    int8_t src_hwheel = (int8_t)HidMouseDescriptorMap_HWheel(report, len, map);

    // X1 の立ち上がりでトラックスクロールモードをトグル
    bool prev_track_scroll_mode = s_track_scroll_mode;
    bool x1_pressed = (src_btn & BTN_X1_MASK) != 0;
    bool x1_prev_pressed = (s_prev_buttons & BTN_X1_MASK) != 0;
    if (x1_pressed && !x1_prev_pressed) {
        s_track_scroll_mode = !s_track_scroll_mode;
    }
    if (s_track_scroll_mode != prev_track_scroll_mode) {
        s_scroll_acc_v = 0;
        s_scroll_acc_h = 0;
    }

    // モード中にX1以外のボタンが押されたら解除し、当該レポートは送信しない
    if (s_track_scroll_mode && (src_btn & BTN_NON_X1_MASK) != 0) {
        s_track_scroll_mode = false;
        s_scroll_acc_v = 0;
        s_scroll_acc_h = 0;
        s_prev_buttons = src_btn;
        gpio_put(LED_PIN, true);
        tuh_hid_receive_report(dev_addr, instance);
        return;
    }

    s_prev_buttons = src_btn;
    gpio_put(LED_PIN, s_track_scroll_mode ? false : true);

    // 出力は Left/Middle/Right のみ
    r.data[0] = (uint8_t)(src_btn & BTN_MASK_OUT);

    if (s_track_scroll_mode) {
        // モード中は移動量を縦横スクロールに変換
        int8_t v_steps = consume_scroll_steps(&s_scroll_acc_v, (int16_t)src_wheel - (int16_t)src_y);
        int8_t h_steps = consume_scroll_steps(&s_scroll_acc_h, (int16_t)src_hwheel + (int16_t)src_x);
        r.data[1] = 0;
        r.data[2] = 0;
        r.data[3] = (uint8_t)v_steps;
        r.data[4] = (uint8_t)h_steps;
    } else {
        r.data[1] = (uint8_t)src_x;
        r.data[2] = (uint8_t)src_y;
        r.data[3] = (uint8_t)src_wheel;
        r.data[4] = (uint8_t)src_hwheel;
    }
    queue_try_add(&s_report_queue, &r);

    tuh_hid_receive_report(dev_addr, instance);
}
void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t instance,
                      uint8_t const *desc_report, uint16_t desc_len)
{
    debug_uart_puts("[HID] mounting...\r\n");

    // デスクリプタを解析してX/Y/Wheel/Buttonのバイト位置を特定
    HidMouseDescriptorMap *map = 
        &s_device_map[dev_addr < DEVICE_MAP_MAX ? dev_addr : 0];
    HidMouseDescriptorMap_Parse(desc_report, desc_len, map);
    debug_uart_puts("[HID] mounted\r\n");

    uint8_t const itf_protocol = tuh_hid_interface_protocol(dev_addr, instance);
    if (itf_protocol == HID_ITF_PROTOCOL_MOUSE) {
        // Boot Interface 対応マウス: Report Protocol に切り替えてホイールを有効化
        // 完了後に tuh_hid_set_protocol_complete_cb -> tuh_hid_receive_report の順で呼ばれる
        tuh_hid_set_protocol(dev_addr, instance, HID_PROTOCOL_REPORT);
    } else {
        // Boot Interface なし: そのまま受信開始
        tuh_hid_receive_report(dev_addr, instance);
    }
}

void tuh_hid_set_protocol_complete_cb(uint8_t dev_addr, uint8_t instance, uint8_t protocol)
{
    (void)protocol;
    debug_uart_puts("[HID] protocol set\r\n");
    tuh_hid_receive_report(dev_addr, instance);
}

void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance)
{
    (void)dev_addr; (void)instance;
    s_track_scroll_mode = false;
    s_prev_buttons = 0;
    s_scroll_acc_v = 0;
    s_scroll_acc_h = 0;
    gpio_put(LED_PIN, true);
}

// ---- Core 1 - PIO USB Host ----

static void Core1Main(void)
{
    for (int i = 0; i < CAPACITY_OF(s_device_map); i++)
    {
        HidMouseDescriptorMap_Init(&s_device_map[i]);
    }

    pio_usb_configuration_t pio_cfg = PIO_USB_DEFAULT_CONFIG;
    pio_cfg.pin_dp = USB_DP_PIN;

    tuh_configure(1, TUH_CFGID_RPI_PIO_USB_CONFIGURATION, &pio_cfg);
    tuh_init(1);

    while (true) {
        tuh_task();
    }
}

// ---- Core 0 - main ----

int main(void)
{
    queue_init(&s_report_queue, sizeof(hid_report_t), REPORT_QUEUE_SIZE);

    debug_uart_init();
    debug_uart_puts("[BOOT] start\r\n");

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // Core0: ハードウェアUSBコントローラでHIDマウスデバイス初期化
    tud_init(0);

    multicore_launch_core1(Core1Main);
    debug_uart_puts("[BOOT] core1 launched\r\n");

    // uint32_t last_blink = 0;
    // bool led_state = false;

    while (true) {
        tud_task();

        hid_report_t r;
        if (tud_hid_ready() && queue_try_remove(&s_report_queue, &r)) {
            tud_hid_report(1, r.data, 5);  // 常に 5 バイト送信
        }

        // uint32_t now = to_ms_since_boot(get_absolute_time());
        // if (now - last_blink >= BLINK_INTERVAL_MS) {
        //     led_state = !led_state;
        //     gpio_put(LED_PIN, led_state);
        //     last_blink = now;
        // }
    }
}


