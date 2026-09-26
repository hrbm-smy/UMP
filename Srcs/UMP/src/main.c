#include <string.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/util/queue.h"
#include "pio_usb.h"
#include "tusb.h"

#define LED_PIN           25
#define BLINK_INTERVAL_MS 1000
#define USB_DP_PIN        0   // D+ = GPIO 0, D- = GPIO 1 (自動)

#define REPORT_QUEUE_SIZE 16
#define REPORT_MAX_LEN    16

typedef struct {
    uint8_t data[REPORT_MAX_LEN];
    uint8_t len;
} hid_report_t;

static queue_t s_report_queue;

// 受信レポートから出力へのマッピング情報
typedef struct {
    uint8_t report_id;    // 0 = なし
    int8_t  btn_byte;     // ボタンバイトオフセット (-1 = 不明)
    int8_t  x_byte;
    int8_t  y_byte;
    int8_t  wheel_byte;
} mouse_map_t;

// dev_addr は 1-based で最大 CFG_TUH_DEVICE_MAX
#define DEVICE_MAP_MAX (CFG_TUH_DEVICE_MAX + 1)
static mouse_map_t s_device_map[DEVICE_MAP_MAX];

// HIDデスクリプタを解析してマウスのバイトマップを構築
static void parse_mouse_descriptor(uint8_t const *desc, uint16_t len, mouse_map_t *map)
{
    map->report_id  = 0;
    map->btn_byte   = -1;
    map->x_byte     = -1;
    map->y_byte     = -1;
    map->wheel_byte = -1;

    uint16_t usage_page = 0;
    uint16_t usage      = 0;
    int32_t  bit_offset = 0;  // report ID除く先頭からのビット位置
    uint8_t  report_size  = 0;
    uint8_t  report_count = 0;
    bool     in_pointer   = false;

    // pending usages for multi-usage items
    uint8_t pending_usages[8];
    uint8_t pending_count = 0;

    uint16_t i = 0;
    while (i < len) {
        uint8_t tag  = desc[i] & 0xFC;
        uint8_t size = desc[i] & 0x03;
        i++;

        uint32_t val = 0;
        for (uint8_t s = 0; s < size; s++) {
            val |= ((uint32_t)desc[i++]) << (8 * s);
        }

        switch (tag) {
            case 0x04: usage_page   = (uint16_t)val; break;  // Usage Page
            case 0x08: usage        = (uint16_t)val;          // Usage
                if (pending_count < 8) pending_usages[pending_count++] = (uint8_t)val;
                break;
            case 0x74: report_size  = (uint8_t)val;  break;  // Report Size
            case 0x94: report_count = (uint8_t)val;  break;  // Report Count
            case 0x84: map->report_id = (uint8_t)val; break; // Report ID
            case 0xA0:                                         // Collection
                if (usage == 0x01) in_pointer = true;
                pending_count = 0;
                break;
            case 0xC0: in_pointer = false; pending_count = 0; break; // End Collection
            case 0x80: {  // Input
                bool is_const = (val & 0x01) != 0;
                if (is_const) {
                    // padding
                } else if (usage_page == 0x09) {
                    // Button page
                    if (map->btn_byte < 0) map->btn_byte = (int8_t)(bit_offset / 8);
                } else if (usage_page == 0x01) {
                    // Generic Desktop
                    uint8_t apply = (pending_count > 0 && pending_count <= report_count)
                                    ? pending_count : report_count;
                    for (uint8_t u = 0; u < apply && u < pending_count; u++) {
                        int8_t byte_pos = (int8_t)((bit_offset + u * report_size) / 8);
                        switch (pending_usages[u]) {
                            case 0x30: map->x_byte     = byte_pos; break;  // X
                            case 0x31: map->y_byte     = byte_pos; break;  // Y
                            case 0x38: map->wheel_byte = byte_pos; break;  // Wheel
                        }
                    }
                }
                bit_offset += report_size * report_count;
                pending_count = 0;
                break;
            }
        }
    }
}

// ---- HID Host コールバック (Core1 から呼ばれる) ----

void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance,
                                uint8_t const *report, uint16_t len)
{
    static bool led = false;
    led = !led;
    gpio_put(LED_PIN, led);

    mouse_map_t const *map = &s_device_map[dev_addr < DEVICE_MAP_MAX ? dev_addr : 0];

    // Report ID がある場合は先頭1バイトをスキップ
    if (map->report_id != 0 && len > 0) {
        report++;
        len--;
    }

    hid_report_t r;
    r.len = 4;
    // デスクリプタ解析結果に基づいてバイトを取り出す
    r.data[0] = (map->btn_byte   >= 0 && map->btn_byte   < (int8_t)len) ? report[map->btn_byte]   : 0;
    r.data[1] = (map->x_byte     >= 0 && map->x_byte     < (int8_t)len) ? report[map->x_byte]     : 0;
    r.data[2] = (map->y_byte     >= 0 && map->y_byte     < (int8_t)len) ? report[map->y_byte]     : 0;
    r.data[3] = (map->wheel_byte >= 0 && map->wheel_byte < (int8_t)len) ? report[map->wheel_byte] : 0;
    queue_try_add(&s_report_queue, &r);

    tuh_hid_receive_report(dev_addr, instance);
}
void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t instance,
                      uint8_t const *desc_report, uint16_t desc_len)
{

    // デスクリプタを解析してX/Y/Wheel/Buttonのバイト位置を特定
    mouse_map_t *map = &s_device_map[dev_addr < DEVICE_MAP_MAX ? dev_addr : 0];
    parse_mouse_descriptor(desc_report, desc_len, map);

    uint8_t const itf_protocol = tuh_hid_interface_protocol(dev_addr, instance);
    if (itf_protocol == HID_ITF_PROTOCOL_MOUSE) {
        // Boot Interface 対応マウス: Report Protocol に切り替えてホイールを有効化
        // 完了後に tuh_hid_set_protocol_complete_cb -> tuh_hid_receive_report の順で呼ばれる
        tuh_hid_set_protocol(dev_addr, instance, HID_PROTOCOL_REPORT);
        gpio_put(LED_PIN, false);
    } else {
        // Boot Interface なし: そのまま受信開始
        tuh_hid_receive_report(dev_addr, instance);
    }
}

void tuh_hid_set_protocol_complete_cb(uint8_t dev_addr, uint8_t instance, uint8_t protocol)
{
    (void)protocol;
    tuh_hid_receive_report(dev_addr, instance);
}

void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance)
{
    (void)dev_addr; (void)instance;
    gpio_put(LED_PIN, true);
}

// ---- Core 1 - PIO USB Host ----

static void Core1Main(void)
{
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

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // Core0: ハードウェアUSBコントローラでHIDマウスデバイス初期化
    tud_init(0);

    multicore_launch_core1(Core1Main);

    uint32_t last_blink = 0;
    bool led_state = false;

    while (true) {
        tud_task();

        hid_report_t r;
        if (tud_hid_ready() && queue_try_remove(&s_report_queue, &r)) {
            tud_hid_report(1, r.data, r.len);
        }

        // uint32_t now = to_ms_since_boot(get_absolute_time());
        // if (now - last_blink >= BLINK_INTERVAL_MS) {
        //     led_state = !led_state;
        //     gpio_put(LED_PIN, led_state);
        //     last_blink = now;
        // }
    }
}


