/** -------------------------------------------------------------------------
 *
 *	@file	HidReportRing.c
 *	@brief	Ring buffer for the HID report packet
 *	@author	H.Someya
 *	@date	2026/10/02
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
#include "HidReportRing.h"

#include <stddef.h>
#include <string.h>

/* --------------------------------------------------------------------------
 *  P R I V A T E S
 */

/* --------------------------------------------------------------------------
 *  P U B L I C   I N T E R F A C E S
 */

/**
 *  @brief HIDレポートリングバッファ 初期化 @n
 *    HIDレポートリングバッファを初期化する。
 *  @param ctxt コンテキスト。
 *  @return なし。
 */
void HidReportRing_Init(
    HidReportRing *ctxt)
{
    if (ctxt != NULL)
    {
        memset(ctxt, 0, sizeof(HidReportRing));

        RawLock_Init(&ctxt->Lock);

        RingedFrames_Init(
            HID_REPORT_RING_DEPTH,
            sizeof(HidReportPacket),
            ctxt->Buffer, &ctxt->Ring);
    }
}

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
    HidReportRing *ctxt)
{
    if (ctxt != NULL)
    {
        if (RawLock_Lock(&ctxt->Lock))
        {
            RingedFrames_Push(
                packet, packetSize, 0LL, &ctxt->Ring);

            RawLock_Unlock(&ctxt->Lock);
        }
    }
}

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
    HidReportRing *ctxt)
{
    int32_t result = 0;

    if (ctxt != NULL)
    {
        if (RawLock_Lock(&ctxt->Lock))
        {
            result = RingedFrames_Pop(
                packet, sizeof(HidReportPacket),
                NULL, &ctxt->Ring);

            RawLock_Unlock(&ctxt->Lock);
        }
    }

    return result;
}
