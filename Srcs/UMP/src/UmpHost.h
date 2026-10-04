#ifndef __UmpHost_H__
#define __UmpHost_H__

/** -------------------------------------------------------------------------
 *
 *	@file	UmpHost.h
 *	@brief	USB Mouse Proxy Host
 *	@author	H.Someya
 *	@date	2026/10/03
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

#include "HidReportDescriptorRing.h"
#include "HidReportRing.h"

/* --------------------------------------------------------------------------
 *  P U B L I C   D E F I N I T I O N S
 */

/* --------------------------------------------------------------------------
 *  P U B L I C   I N T E R F A C E S
 */

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * USB Mouse Proxy Host
     */
    typedef struct _UmpHost
    {
        HidReportDescriptorRing DescRing;
        HidReportRing ReportRing;
    } UmpHost;

    /**
     *  @brief USBマウスプロキシホスト インスタンス取得 @n
     *    インスタンスを取得する。
     *  @return インスタンス。
     *    現状、TinyUSBの仕様(コールバックの仕様)から、複数インスタンスは動かせない。
     *    このインスタンスを使用すること。
     */
    UmpHost *UmpHost_Instance(void);

    /**
     *  @brief USBマウスプロキシホスト 初期化 @n
     *    このモジュールを初期化する。
     *  @param ctxt コンテキスト。
     *    現状、TinyUSBの仕様(コールバックの仕様)から、複数インスタンスは動かせない。
     *    UmpHost_Instance()を使用すること。
     *  @return なし。
     */
    void UmpHost_Init(
        UmpHost *ctxt);

    /**
     *  @brief USBマウスプロキシホスト 駆動 @n
     *    このモジュールを駆動する。
     *  @param ctxt コンテキスト。
     *    現状、TinyUSBの仕様(コールバックの仕様)から、複数インスタンスは動かせない。
     *    UmpHost_Instance()を使用すること。
     *  @return なし。
     */
    void UmpHost_Drive(
        UmpHost *ctxt);

#ifdef __cplusplus
}
#endif

#endif
