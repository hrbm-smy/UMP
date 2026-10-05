#ifndef __UmpDevice_H__
#define __UmpDevice_H__

/** -------------------------------------------------------------------------
 *
 *	@file	UmpDevice.h
 *	@brief	USB Mouse Proxy Device
 *	@author	H.Someya
 *	@date	2026/10/04
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

#include "Map.h"
#include "RingedFrames.h"
#include "SchmittTrigger.h"
#include "HidReportDescriptorRing.h"
#include "HidReportRing.h"
#include "HidMouseDescriptorMap.h"
#include "UmpConfig.h"

/* --------------------------------------------------------------------------
 *  P U B L I C   D E F I N I T I O N S
 */

/**
 * リポートディスクリプタ マップ 最大容量
 */
#define UMPDEVICE_DESC_CAP (4)

/**
 * リポート履歴 最大容量
 */
#define UMPDEVICE_REPORTHIST_CAP (32)

/* --------------------------------------------------------------------------
 *  P U B L I C   I N T E R F A C E S
 */

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * USB Mouse Proxy Device
     */
    typedef struct _UmpDevice
    {
        /**
         * リポートディスクリプタ マップ
         */
        Map DescMap;
        /**
         * リポートディスクリプタ マップ要素リスト
         */
        MapElm DescMapElms[UMPDEVICE_DESC_CAP];

        /**
         * HID マウス ディスクリプタ マップ リスト
         */
        HidMouseDescriptorMap MouseDescs[UMPDEVICE_DESC_CAP];

        /**
         * リポート履歴 リングバッファ
         */
        RingedFrames ReportHistRing;
        /**
         * リポート履歴 リングバッファ 用 バッファ
         */
        int32_t ReportHistBuffer[RF_NEEDED_BUFFER_WORDS(
            UMPDEVICE_REPORTHIST_CAP,
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
    } UmpDevice;

    /**
     * USB Mouse Proxy Device Drive inputs
     */
    typedef struct _UmpDevice_Drive_In
    {
        const UmpConfig *Config;
        HidReportDescriptorRing *DescRing;
        HidReportRing *ReportRing;
    } UmpDevice_Drive_In;

    /**
     *  @brief USBマウスプロキシデバイス インスタンス取得 @n
     *    インスタンスを取得する。
     *  @return インスタンス。
     *    現状、TinyUSBの仕様(コールバックの仕様)から、複数インスタンスは動かせない。
     *    このインスタンスを使用すること。
     */
    UmpDevice *UmpDevice_Instance(void);

    /**
     *  @brief USBマウスプロキシデバイス 初期化 @n
     *    このモジュールを初期化する。
     *  @param ctxt コンテキスト。
     *    現状、TinyUSBの仕様(コールバックの仕様)から、複数インスタンスは動かせない。
     *    UmpDevice_Instance()を使用すること。
     *  @return なし。
     */
    void UmpDevice_Init(
        UmpDevice *ctxt);

    /**
     *  @brief USBマウスプロキシデバイス 駆動 @n
     *    このモジュールを駆動する。
     *  @param inputs このモジュールへの入力。
     *  @param ctxt コンテキスト。
     *    現状、TinyUSBの仕様(コールバックの仕様)から、複数インスタンスは動かせない。
     *    UmpDevice_Instance()を使用すること。
     *  @return なし。
     */
    void UmpDevice_Drive(
        const UmpDevice_Drive_In *inputs,
        UmpDevice *ctxt);

#ifdef __cplusplus
}
#endif

#endif
