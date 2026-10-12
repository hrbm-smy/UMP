/** -------------------------------------------------------------------------
 *
 *	@file	HidReportDescriptorRing.c
 *	@brief	Ring buffer for the HID report descriptor packet
 *	@author	H.Someya
 *	@date	2026/10/01
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
#include "HidReportDescriptorRing.h"

#include <stddef.h>
#include <string.h>

/* --------------------------------------------------------------------------
 *  P R I V A T E S
 */

/* --------------------------------------------------------------------------
 *  P U B L I C   I N T E R F A C E S
 */

/**
 *  @brief HIDレポートディスクリプタリングバッファ 初期化 @n
 *    HIDレポートディスクリプタリングバッファを初期化する。
 *  @param ctxt コンテキスト。
 *  @return なし。
 */
void HidReportDescriptorRing_Init(
	HidReportDescriptorRing *ctxt)
{
	if (ctxt != NULL)
	{
		memset(ctxt, 0, sizeof(HidReportDescriptorRing));

		RawLock_Init(&ctxt->Lock);

		RingedFrames_Init(
			HID_REPORT_DESC_RING_DEPTH,
			sizeof(HidReportDescriptorPacket),
			ctxt->Buffer, &ctxt->Ring);
	}
}

/**
 *  @brief HIDレポートディスクリプタリングバッファ Push @n
 *    HIDレポートディスクリプタパケットを、
 *    HIDレポートディスクリプタリングバッファに、Pushする。
 *  @param packet Pushするパケット。
 *  @param packetSize パケットのサイズ。
 *  @param ctxt コンテキスト。
 *  @return なし。
 */
void HidReportDescriptorRing_Push(
	const HidReportDescriptorPacket *packet,
	int32_t packetSize,
	HidReportDescriptorRing *ctxt)
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
 *  @brief HIDレポートディスクリプタリングバッファ Pop @n
 *    HIDレポートディスクリプタパケットを、
 *    HIDレポートディスクリプタリングバッファから、Popする。
 *  @param packet Popしたパケットの格納先。
 *  @param ctxt コンテキスト。
 *  @return Popしたサイズ。
 */
int32_t HidReportDescriptorRing_Pop(
	HidReportDescriptorPacket *packet,
	HidReportDescriptorRing *ctxt)
{
	int32_t result = 0;

	if (ctxt != NULL)
	{
		if (RawLock_Lock(&ctxt->Lock))
		{
			result = RingedFrames_Pop(
				packet, sizeof(HidReportDescriptorPacket),
				NULL, &ctxt->Ring);

			RawLock_Unlock(&ctxt->Lock);
		}
	}

	return result;
}
