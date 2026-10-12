#ifndef __HidReportDescriptorRing_H__
#define __HidReportDescriptorRing_H__

/** -------------------------------------------------------------------------
 *
 *	@file	HidReportDescriptorRing.h
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

#include <stdint.h>
#include "RingedFrames.h"
#include "RawLock.h"
#include "HidReportDescriptorPacket.h"

/* --------------------------------------------------------------------------
 *  P U B L I C   D E F I N I T I O N S
 */

/**
 * リングバッファの深さ
 */
#define HID_REPORT_DESC_RING_DEPTH (4)

/**
 * パラメータ付きdefine
 * @param p1 パラメータ1
 * @param p2 パラメータ2
 */
#define TEMPLATE_PARAMETERIZED_DEFINE(p1, p2) \
	((p1) + (p2))

/**
 * @name defineグループ
 *
 * 複数のdefineをグループにする。
 * enumでいいんではないか、という気もするが、何かに使えるかもしれない。
 *
 * @{
 */
#define TEMPLATE_GROUP1_VALUE1 (0) /**< 値1 */
#define TEMPLATE_GROUP1_VALUE2 (2) /**< 値2 */
#define TEMPLATE_GROUP1_VALUE3 (3) /**< 値3 */

/** @}*/

/* --------------------------------------------------------------------------
 *  P U B L I C   I N T E R F A C E S
 */

#ifdef __cplusplus
extern "C"
{
#endif

	/**
	 * Ring buffer for the HID report descriptor packet
	 */
	typedef struct _HidReportDescriptorRing
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
			HID_REPORT_DESC_RING_DEPTH,
			sizeof(HidReportDescriptorPacket))];

	} HidReportDescriptorRing;

	/**
	 *  @brief HIDレポートディスクリプタリングバッファ 初期化 @n
	 *    HIDレポートディスクリプタリングバッファを初期化する。
	 *  @param ctxt コンテキスト。
	 *  @return なし。
	 */
	void HidReportDescriptorRing_Init(
		HidReportDescriptorRing *ctxt);

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
		HidReportDescriptorRing *ctxt);

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
		HidReportDescriptorRing *ctxt);

#ifdef __cplusplus
}
#endif

#endif
