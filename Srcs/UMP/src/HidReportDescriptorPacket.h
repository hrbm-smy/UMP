#ifndef __HidReportDescriptorPacket_H__
#define __HidReportDescriptorPacket_H__

/** -------------------------------------------------------------------------
 *
 *	@file	HidReportDescriptorPacket.h
 *	@brief	HID report descriptor packet
 *	@author	H.Someya
 *	@date	2026/09/30
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

/* --------------------------------------------------------------------------
 *  P U B L I C   D E F I N I T I O N S
 */

#define CFG_REPORT_DESC_CAP (512)

/* --------------------------------------------------------------------------
 *  P U B L I C   I N T E R F A C E S
 */

#ifdef __cplusplus
extern "C"
{
#endif

	/**
	 * HID report descriptor packet
	 */
	typedef struct _HidReportDescriptorPacket
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
		 * Descriptor length
		 */
		uint16_t DescLength;
		/**
		 * Report descriptor
		 */
		uint8_t ReportDesc[CFG_REPORT_DESC_CAP];
	} HidReportDescriptorPacket;

	/**
	 *  @brief HIDレポートディスクリプタパケット 初期化 @n
	 *    HIDレポートディスクリプタパケットを初期化する。
	 *  @param ctxt コンテキスト。
	 *  @return なし。
	 */
	void HidReportDescriptorPacket_Init(
		HidReportDescriptorPacket *ctxt);

	/**
	 *  @brief HIDレポートディスクリプタパケット セット @n
	 *    HIDレポートディスクリプタパケットに値をセットする。
	 *  @param address アドレス。
	 *  @param instance インスタンス。
	 *  @param reportDesc レポートディスクリプタ。
	 *  @param descLength レポートディスクリプタのバイト長。
	 *  @param ctxt コンテキスト。
	 *  @return パケットのバイト長。リングバッファにプッシュするサイズ。
	 */
	int32_t HidReportDescriptorPacket_Set(
		uint8_t address,
		uint8_t instance,
		const uint8_t *reportDesc,
		uint16_t descLength,
		HidReportDescriptorPacket *ctxt);

#ifdef __cplusplus
}
#endif

#endif
