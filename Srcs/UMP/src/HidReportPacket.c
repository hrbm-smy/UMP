/** -------------------------------------------------------------------------
 *
 *	@file	HidReportPacket.c
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
#include "HidReportPacket.h"

#include <stddef.h>
#include <string.h>
#include "MinMax.h"

/* --------------------------------------------------------------------------
 *  P R I V A T E S
 */

/* --------------------------------------------------------------------------
 *  P U B L I C   I N T E R F A C E S
 */

/**
 *  @brief HIDレポートパケット 初期化 @n
 *    HIDレポートパケットを初期化する。
 *  @param ctxt コンテキスト。
 *  @return なし。
 */
void HidReportPacket_Init(
    HidReportPacket *ctxt)
{
    if (ctxt != NULL)
    {
        memset(ctxt, 0, sizeof(HidReportPacket));
    }
}

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
    HidReportPacket *ctxt)
{
    int32_t result = 0;

    if (ctxt != NULL)
    {
        // 情報を保存
        ctxt->Address = address;
        ctxt->Instance = instance;

        // 詰め込めるだけの長さにする
        uint16_t len = MinS32(reportLength, sizeof ctxt->Report);
        ctxt->ReportLength = len;

        // レポートを記録
        memcpy(ctxt->Report, report, len);

        // 長さはレポートの始まり位置+レポートの長さ
        result = offsetof(HidReportPacket, Report) + len;
    }

    return result;
}
