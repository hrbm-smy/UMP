#ifndef _TUSB_CONFIG_H_
#define _TUSB_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

#define CFG_TUSB_OS             OPT_OS_PICO
#define CFG_TUSB_DEBUG          0

// Device (Core0 - native USB HID Mouse)
#define CFG_TUD_ENABLED         1
#define CFG_TUD_HID             1
#define CFG_TUD_HID_EP_BUFSIZE  16

// Host (Core1 - PIO USB)
#define CFG_TUH_ENABLED         1
#define CFG_TUH_RPI_PIO_USB     1
#define CFG_TUH_MAX_SPEED       OPT_MODE_DEFAULT_SPEED

// HID
#define CFG_TUH_HID             4
#define CFG_TUH_HID_EP_BUFSIZE  64

// Hub support
#define CFG_TUH_HUB             1

// Device max
#define CFG_TUH_DEVICE_MAX      (3*CFG_TUH_HUB + 1)

#ifdef __cplusplus
}
#endif

#endif // _TUSB_CONFIG_H_
