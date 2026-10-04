#ifndef __HidReportPacket_H__
#define __HidReportPacket_H__

/** -------------------------------------------------------------------------
 *
 *	@file	HidReportPacket.h
 *	@brief	HID report packet
 *	@author	H.Someya
 *	@date	2026/10/02
 *
 */
/*
MIT License

Copyright (c) 2021 Hirobumi Someya

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

#include <stdint.h>

/* --------------------------------------------------------------------------
 *  P U B L I C   D E F I N I T I O N S
 */

#define CFG_REPORT_PACKET_CAP (256)

/* --------------------------------------------------------------------------
 *  P U B L I C   I N T E R F A C E S
 */

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * HID report packet
     */
    typedef struct _HidReportPacket
    {
        /**
         * Address
         */
        uint8_t Address;
        /**
         * Instance
         */
        uint8_t Instance;
        /**
         * Report length
         */
        uint16_t ReportLength;
        /**
         * Report
         */
        uint8_t Report[CFG_REPORT_PACKET_CAP];
    } HidReportPacket;

    /**
     *  @brief HIDレポートパケット 初期化 @n
     *    HIDレポートパケットを初期化する。
     *  @param ctxt コンテキスト。
     *  @return なし。
     */
    void HidReportPacket_Init(
        HidReportPacket *ctxt);

    /**
     *  @brief HIDレポートパケット セット @n
     *    HIDレポートパケットに値をセットする。
     *  @param address アドレス。
     *  @param instance インスタンス。
     *  @param report レポート。
     *  @param reportLength レポートのバイト長。
     *  @param ctxt コンテキスト。
     *  @return パケットのバイト長。リングバッファにプッシュするサイズ。
     */
    int32_t HidReportPacket_Set(
        uint8_t address,
        uint8_t instance,
        const uint8_t *report,
        uint16_t reportLength,
        HidReportPacket *ctxt);

#ifdef __cplusplus
}
#endif

#endif
