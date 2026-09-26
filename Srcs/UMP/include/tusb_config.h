#ifndef _TUSB_CONFIG_H_
#define _TUSB_CONFIG_H_

// Common
#define CFG_TUSB_MCU              OPT_MCU_RP2040
#define CFG_TUSB_OS               OPT_OS_PICO
#define CFG_TUSB_DEBUG            0

// Device: Core0 / Native USB
#define CFG_TUD_ENABLED           1
#define CFG_TUD_HID               1
#define CFG_TUD_ENDPOINT0_SIZE    64
#define CFG_TUD_HID_EP_BUFSIZE    64

// Host: Core1 / PIO USB
#define CFG_TUH_ENABLED           1
#define CFG_TUH_RPI_PIO_USB       1
#define CFG_TUH_MAX_SPEED         OPT_MODE_DEFAULT_SPEED

// HID Host
#define CFG_TUH_HID               4
#define CFG_TUH_HID_EP_BUFSIZE    64

// Enumeration
#define CFG_TUH_ENUMERATION_BUFSIZE 512

// No USB Hub
#define CFG_TUH_HUB               0
#define CFG_TUH_DEVICE_MAX        1

#endif
