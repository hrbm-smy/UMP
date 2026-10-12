#ifndef __UmpMouse_H__
#define __UmpMouse_H__

/** -------------------------------------------------------------------------
 *
 *	@file	UmpMouse.h
 *	@brief	UMP Mouse context
 *	@author	H.Someya
 *	@date	2026/10/12
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
#include "SchmittTrigger.h"

#include "HidMouseDescriptorMap.h"
#include "HidReportPacket.h"

/* --------------------------------------------------------------------------
 *  P U B L I C   D E F I N I T I O N S
 */

/**
 * リポート履歴 最大容量
 */
#define UMPMOUSE_REPORTHIST_CAP (32)

/* --------------------------------------------------------------------------
 *  P U B L I C   I N T E R F A C E S
 */

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * UMP Mouse context
     */
    typedef struct _UmpMouse
    {
        /**
         * HID マウス ディスクリプタ マップ
         */
        HidMouseDescriptorMap DescMap;

        /**
         * リポート履歴 リングバッファ
         */
        RingedFrames ReportHistRing;
        /**
         * リポート履歴 リングバッファ 用 バッファ
         */
        int32_t ReportHistBuffer[RF_NEEDED_BUFFER_WORDS(
            UMPMOUSE_REPORTHIST_CAP,
            sizeof(HidReportPacket))];

        /**
         * ボタンの現在状態。
         */
        uint32_t ButtonState;

        /**
         * 動作モード。
         * 0:通常。
         * 非0:各モード。UMPDEVICE_MODE_???を参照。
         */
        int32_t Mode;

        /**
         * スクロール累積。
         */
        int32_t ScrollAcc;

        /**
         * スクロール方向シュミットトリガ。
         * State=0:垂直(縦)、State=非0:水平(横)
         */
        SchmittTrigger DirectionSchmitt;
    } UmpMouse;

    /**
     *  @brief UMPマウスコンテキスト 初期化 @n
     *    このモジュールを初期化する。
     *  @param ctxt コンテキスト。
     *  @return なし。
     */
    void UmpMouse_Init(
        UmpMouse *ctxt);

#ifdef __cplusplus
}
#endif

#endif
