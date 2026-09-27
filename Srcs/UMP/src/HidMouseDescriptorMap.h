#ifndef __HidMouseDescriptorMap_H__
#define __HidMouseDescriptorMap_H__

/** -------------------------------------------------------------------------
 *
 *	@file	HidMouseDescriptorMap.h
 *	@brief	HID Mouse descriptor map
 *	@author	H.Someya
 *	@date	2026/09/27
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
     * HID マウス ディスクリプタ マップ
     */
    typedef struct _HidMouseDescriptorMap
    {
        /**
         * Report ID
         * 0 = なし
         */
        uint8_t ReportId;
        /**
         * ボタンデータ位置インデックス
         * 負 = 不明
         */
        int16_t ButtonIndex;
        /**
         * ボタンデータサイズ
         */
        int16_t ButtonSize;
        /**
         * Xデータ位置インデックス
         * 負 = 不明
         */
        int16_t XIndex;
        /**
         * Xデータサイズ
         */
        int16_t XSize;
        /**
         * Yデータ位置インデックス
         * 負 = 不明
         */
        int16_t YIndex;
        /**
         * Yデータサイズ
         */
        int16_t YSize;
        /**
         * 垂直ホイール(通常のホイール)データ位置インデックス
         * 負 = 不明
         */
        int16_t VWheelIndex;
        /**
         * 垂直ホイール(通常のホイール)データサイズ
         */
        int16_t VWheelSize;
        /**
         * 水平ホイール(横ホイール)データ位置インデックス
         * 負 = 不明
         */
        int16_t HWheelIndex;
        /**
         * 水平ホイール(横ホイール)データサイズ
         */
        int16_t HWheelSize;
    } HidMouseDescriptorMap;

    /**
     *  @brief HID マウス ディスクリプタ マップ 初期化 @n
     *    このモジュールを初期化する。
     *  @param ctxt コンテキスト。
     *  @return なし。
     */
    void HidMouseDescriptorMap_Init(
        HidMouseDescriptorMap *ctxt);

    /**
     *  @brief HID マウス ディスクリプタ マップ 解析 @n
     *    ディスクリプタを解析し、データを取得するための準備を行う。
     *  @param desc ディスクリプタ。
     *  @param len ディスクリプタの長さ。
     *  @param ctxt コンテキスト。
     *  @return なし。
     */
    void HidMouseDescriptorMap_Parse(
        const uint8_t *desc, uint16_t len,
        HidMouseDescriptorMap *ctxt);

    /**
     *  @brief nバイトデコード @n
     *    nバイトの値をデコードする。
     *    デコード元バッファは、Decoders_CanDecodeによりチェックしておくこと。
     *    ここではデコード元バッファのチェックは行わない。
     *  @param index デコード元のインデックス。
     *  @param n デコードするバイト数。
     *  @param src デコード元バッファ。
     *  @param bigEndian 非0でBig Endianでエンコードする。
     *  @return デコードした値。
     */
    uint32_t Decoders_nAt(
        int32_t index, int32_t n,
        const void *src,
        int bigEndian);

    /**
     *  @brief HID マウス ディスクリプタ マップ ボタン取得 @n
     *    レポートからボタン(ビット列)データを取得する。
     *  @param report レポート。
     *  @param len レポートの長さ。
     *  @param ctxt コンテキスト。
     *  @return ボタン(ビット列)データ。
     */
    uint32_t HidMouseDescriptorMap_Buttons(
        const uint8_t *report, uint16_t len,
        const HidMouseDescriptorMap *ctxt);

    /**
     *  @brief HID マウス ディスクリプタ マップ X取得 @n
     *    レポートからX(マウスの水平方向移動量)データを取得する。
     *  @param report レポート。
     *  @param len レポートの長さ。
     *  @param ctxt コンテキスト。
     *  @return X(マウスの水平方向移動量)データ。
     */
    int32_t HidMouseDescriptorMap_X(
        const uint8_t *report, uint16_t len,
        const HidMouseDescriptorMap *ctxt);

    /**
     *  @brief HID マウス ディスクリプタ マップ Y取得 @n
     *    レポートからY(マウスの垂直方向移動量)データを取得する。
     *  @param report レポート。
     *  @param len レポートの長さ。
     *  @param ctxt コンテキスト。
     *  @return Y(マウスの垂直方向移動量)データ。
     */
    int32_t HidMouseDescriptorMap_Y(
        const uint8_t *report, uint16_t len,
        const HidMouseDescriptorMap *ctxt);

    /**
     *  @brief HID マウス ディスクリプタ マップ 垂直ホイール取得 @n
     *    レポートから垂直ホイール移動量データを取得する。
     *  @param report レポート。
     *  @param len レポートの長さ。
     *  @param ctxt コンテキスト。
     *  @return 垂直ホイール移動量データ。
     */
    int32_t HidMouseDescriptorMap_VWheel(
        const uint8_t *report, uint16_t len,
        const HidMouseDescriptorMap *ctxt);

    /**
     *  @brief HID マウス ディスクリプタ マップ 水平ホイール取得 @n
     *    レポートから水平ホイール移動量データを取得する。
     *  @param report レポート。
     *  @param len レポートの長さ。
     *  @param ctxt コンテキスト。
     *  @return 水平ホイール移動量データ。
     */
    int32_t HidMouseDescriptorMap_HWheel(
        const uint8_t *report, uint16_t len,
        const HidMouseDescriptorMap *ctxt);

#ifdef __cplusplus
}
#endif

#endif
