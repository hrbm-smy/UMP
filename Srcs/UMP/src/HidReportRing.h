#ifndef __HidReportRing_H__
#define __HidReportRing_H__

/** -------------------------------------------------------------------------
 *
 *	@file	HidReportRing.h
 *	@brief	Ring buffer for the HID report packet
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
#include "RingedFrames.h"
#include "RawLock.h"
#include "HidReportPacket.h"

/* --------------------------------------------------------------------------
 *  P U B L I C   D E F I N I T I O N S
 */

/**
 * リングバッファの深さ
 */
#define HID_REPORT_RING_DEPTH (16)

/* --------------------------------------------------------------------------
 *  P U B L I C   I N T E R F A C E S
 */

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * Ring buffer for the HID report packet
     */
    typedef struct _HidReportRing
    {
        /**
         * 低レベル排他制御
         */
        RawLock Lock;

        /**
         * フレームリングバッファ
         */
        RingedFrames Ring;
        /**
         * フレームリングバッファ用バッファ
         */
        int32_t Buffer[RF_NEEDED_BUFFER_WORDS(
            HID_REPORT_RING_DEPTH,
            sizeof(HidReportPacket))];

    } HidReportRing;

    /**
     *  @brief HIDレポートリングバッファ 初期化 @n
     *    HIDレポートリングバッファを初期化する。
     *  @param ctxt コンテキスト。
     *  @return なし。
     */
    void HidReportRing_Init(
        HidReportRing *ctxt);

    /**
     *  @brief HIDレポートリングバッファ Push @n
     *    HIDレポートパケットを、
     *    HIDレポートリングバッファに、Pushする。
     *  @param packet Pushするパケット。
     *  @param packetSize パケットのサイズ。
     *  @param ctxt コンテキスト。
     *  @return なし。
     */
    void HidReportRing_Push(
        const HidReportPacket *packet,
        int32_t packetSize,
        HidReportRing *ctxt);

    /**
     *  @brief HIDレポートリングバッファ Pop @n
     *    HIDレポートパケットを、
     *    HIDレポートリングバッファから、Popする。
     *  @param packet Popしたパケットの格納先。
     *  @param ctxt コンテキスト。
     *  @return Popしたサイズ。
     */
    int32_t HidReportRing_Pop(
        HidReportPacket *packet,
        HidReportRing *ctxt);

#ifdef __cplusplus
}
#endif

#endif
