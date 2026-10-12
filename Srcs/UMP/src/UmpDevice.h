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

#include "Map.h"
#include "HidReportDescriptorRing.h"
#include "HidReportRing.h"
#include "UmpMouse.h"
#include "UmpConfig.h"

/* --------------------------------------------------------------------------
 *  P U B L I C   D E F I N I T I O N S
 */

/**
 * マウス 最大容量
 */
#define UMPDEVICE_MICE_CAP (4)

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
         * マウス マップ
         */
        Map MouseMap;
        /**
         * マウス マップ 要素リスト
         */
        MapElm MouseMapElms[UMPDEVICE_MICE_CAP];

        /**
         * マウス リスト
         */
        UmpMouse Mice[UMPDEVICE_MICE_CAP];

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
