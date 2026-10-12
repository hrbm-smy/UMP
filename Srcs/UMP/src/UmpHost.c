/** -------------------------------------------------------------------------
 *
 *	@file	UmpHost.c
 *	@brief	USB Mouse Proxy Host
 *	@author	H.Someya
 *	@date	2026/10/03
 *
 */
/*
MIT License

Copyright (c) 2026 Hirobumi Someya

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include "UmpHost.h"

#include <stdio.h>
#include <stdbool.h> // Needed for pio_usb.h

#include "pio_usb.h"
#include "tusb.h"

#include "LTika.h"
#include "DebugUart.h"

/* --------------------------------------------------------------------------
 *  P R I V A T E S
 */

/**
 * D+ = GPIO 0, D- = GPIO 1 (自動)
 */
#define USB_DP_PIN (0)

/**
 * インスタンス。
 */
static UmpHost _instance;

void tuh_hid_mount_cb(
    uint8_t dev_addr, uint8_t instance,
    uint8_t const *desc_report, uint16_t desc_len)
{
    UmpHost *ctxt = UmpHost_Instance();

    // ディスクリプタパケットにセット
    HidReportDescriptorPacket packet;
    HidReportDescriptorPacket_Init(&packet);
    int32_t packSize = HidReportDescriptorPacket_Set(
        dev_addr, instance, desc_report, desc_len,
        &packet);

    // リングバッファに放り込む
    HidReportDescriptorRing_Push(&packet, packSize, &ctxt->DescRing);

    // Report Protocol に切り替え(たい)
    // 機能をたくさん持っているならそれなりに生かしてあげたい
    uint8_t const prot = tuh_hid_interface_protocol(dev_addr, instance);
    if (prot == HID_ITF_PROTOCOL_MOUSE)
    {
        // Boot Interface 対応マウス: Report Protocol に切り替えてホイールを有効化
        // 完了後に tuh_hid_set_protocol_complete_cb ->
        //   tuh_hid_receive_report の順で呼ばれる
        tuh_hid_set_protocol(dev_addr, instance, HID_PROTOCOL_REPORT);
    }
    else
    {
#if (DEBUGUART_ENABLED == 1)
        char msg[128];
        snprintf(
            msg, sizeof(msg),
            "[HID] Mouse without Boot Interface support was mounted: "
            "Address=0x%x Instance=0x%x\r\n",
            dev_addr, instance);
        DEBUGUART_PUTS(msg);
#endif

        // Boot Interface なし: そのまま受信開始
        tuh_hid_receive_report(dev_addr, instance);
    }
}

void tuh_hid_set_protocol_complete_cb(
    uint8_t dev_addr, uint8_t instance, uint8_t protocol)
{
#if (DEBUGUART_ENABLED == 1)
    char msg[128];
    snprintf(
        msg, sizeof(msg),
        "[HID] Boot mouse was mounted: "
        "Address=0x%x Instance=0x%x Protocol=0x%x\r\n",
        dev_addr, instance, protocol);
    DEBUGUART_PUTS(msg);
#endif

    // レポート受信指示
    tuh_hid_receive_report(dev_addr, instance);
}

void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance)
{
    (void)dev_addr;
    (void)instance;
    LTika_Set(0);
}

void tuh_hid_report_received_cb(
    uint8_t dev_addr, uint8_t instance,
    uint8_t const *report, uint16_t len)
{
    UmpHost *ctxt = UmpHost_Instance();

    // レポートパケットにセット
    HidReportPacket packet;
    HidReportPacket_Init(&packet);
    int32_t packSize = HidReportPacket_Set(
        dev_addr, instance, report, len,
        &packet);

    // リングバッファに放り込む
    HidReportRing_Push(&packet, packSize, &ctxt->ReportRing);

    // 次のレポート受信指示
    tuh_hid_receive_report(dev_addr, instance);
}

/* --------------------------------------------------------------------------
 *  P U B L I C   I N T E R F A C E S
 */

/**
 *  @brief USBマウスプロキシホスト インスタンス取得 @n
 *    インスタンスを取得する。
 *  @return インスタンス。
 *    現状、TinyUSBの仕様(コールバックの仕様)から、複数インスタンスは動かせない。
 *    このインスタンスを使用すること。
 */
UmpHost *UmpHost_Instance(void)
{
    return &_instance;
}

/**
 *  @brief USBマウスプロキシホスト 初期化 @n
 *    このモジュールを初期化する。
 *  @param ctxt コンテキスト。
 *    現状、TinyUSBの仕様(コールバックの仕様)から、複数インスタンスは動かせない。
 *    UmpHost_Instance()を使用すること。
 *  @return なし。
 */
void UmpHost_Init(
    UmpHost *ctxt)
{
    if (ctxt != NULL)
    {
        memset(ctxt, 0, sizeof(UmpHost));

        HidReportDescriptorRing_Init(&ctxt->DescRing);
        HidReportRing_Init(&ctxt->ReportRing);

        // PIO USB を初期化
        pio_usb_configuration_t pio_cfg = PIO_USB_DEFAULT_CONFIG;
        pio_cfg.pin_dp = USB_DP_PIN;
        tuh_configure(1, TUH_CFGID_RPI_PIO_USB_CONFIGURATION, &pio_cfg);
        tuh_init(1);
    }
}

/**
 *  @brief USBマウスプロキシホスト 駆動 @n
 *    このモジュールを駆動する。
 *  @param ctxt コンテキスト。
 *    現状、TinyUSBの仕様(コールバックの仕様)から、複数インスタンスは動かせない。
 *    UmpHost_Instance()を使用すること。
 *  @return なし。
 */
void UmpHost_Drive(
    UmpHost *ctxt)
{
    if (ctxt != NULL)
    {
        tuh_task();
    }
}
