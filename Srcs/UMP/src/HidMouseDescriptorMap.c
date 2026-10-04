/** -------------------------------------------------------------------------
 *
 *	@file	HidMouseDescriptorMap.c
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
#include "HidMouseDescriptorMap.h"

#include <stddef.h>
#include <string.h>
#include "Decoders.h"

/* --------------------------------------------------------------------------
 *  P R I V A T E S
 */

/* --------------------------------------------------------------------------
 *  P U B L I C   I N T E R F A C E S
 */

/**
 *  @brief HID マウス ディスクリプタ マップ 初期化 @n
 *    このモジュールを初期化する。
 *  @param ctxt コンテキスト。
 *  @return なし。
 */
void HidMouseDescriptorMap_Init(
    HidMouseDescriptorMap *ctxt)
{
    if (ctxt != NULL)
    {
        // 初期化
        memset(ctxt, 0, sizeof(HidMouseDescriptorMap));
        ctxt->ReportId = 0;
        ctxt->ButtonIndex = -1;
        ctxt->XIndex = -1;
        ctxt->YIndex = -1;
        ctxt->VWheelIndex = -1;
        ctxt->HWheelIndex = -1;
    }
}

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
    HidMouseDescriptorMap *ctxt)
{
    if (ctxt != NULL)
    {
        HidMouseDescriptorMap_Init(ctxt);

        uint16_t usage_page = 0;
        int32_t bit_offset = 0; // report ID除く先頭からのビット位置
        uint8_t report_size = 0;
        uint8_t report_count = 0;

        // pending usages for multi-usage items (1/2-byte usage values)
        uint16_t pending_usages[8];
        uint8_t pending_count = 0;

        uint16_t i = 0;
        while (i < len)
        {
            // タグが上位6ビット、サイズが下位2ビット
            uint8_t tag = desc[i] & 0xFC;
            uint8_t size_code = desc[i] & 0x03;
            uint8_t size = (size_code == 3) ? 4 : size_code;
            i++;

            // サイズは3バイトっちゅうこともあるだろうから、
            // このデコードはしょうがない
            uint32_t val = 0;

            // 破損したディスクリプタでの範囲外アクセス防止
            if ((uint32_t)i + size > len)
            {
                break;
            }

            for (uint8_t s = 0; s < size; s++)
            {
                val |= ((uint32_t)desc[i++]) << (8 * s);
            }

            if (tag == 0x04) // Usage Page
            {
                usage_page = (uint16_t)val;
            }
            else if (tag == 0x08) // Usage
            {
                if (pending_count < 8)
                {
                    pending_usages[pending_count++] = (uint16_t)val;
                }
            }
            else if (tag == 0x74) // Report Size
            {
                report_size = (uint8_t)val;
            }
            else if (tag == 0x94) // Report Count
            {
                report_count = (uint8_t)val;
            }
            else if (tag == 0x84) // Report ID
            {
                ctxt->ReportId = (uint8_t)val;
            }
            else if (tag == 0xA0) // Collection
            {
                pending_count = 0;
            }
            else if (tag == 0xC0) // End Collection
            {
                pending_count = 0;
            }
            else if (tag == 0x80) // Input
            {
                int is_const = (val & 0x01) != 0;
                if (is_const)
                {
                    // padding
                }
                else if (usage_page == 0x09)
                {
                    // Button page
                    if (ctxt->ButtonIndex < 0)
                    {
                        ctxt->ButtonIndex = (int16_t)(bit_offset / 8);
                        ctxt->ButtonSize = report_size * report_count;
                    }
                }
                else if ((usage_page == 0x01) ||
                         (usage_page == 0x0C))
                {
                    // Generic Desktop / Consumer (AC Pan)
                    uint8_t apply = report_count;
                    if ((pending_count > 0) &&
                        (pending_count <= report_count))
                    {
                        apply = pending_count;
                    }

                    for (uint8_t u = 0; u < apply && u < pending_count; u++)
                    {
                        int byte_pos = ((bit_offset + u * report_size) / 8);
                        if (usage_page == 0x01)
                        {
                            switch (pending_usages[u])
                            {
                            case 0x30: // X
                                ctxt->XIndex = (int16_t)byte_pos;
                                ctxt->XSize = report_size;
                                break;
                            case 0x31: // Y
                                ctxt->YIndex = (int16_t)byte_pos;
                                ctxt->YSize = report_size;
                                break;
                            case 0x38: // Wheel
                                ctxt->VWheelIndex = (int16_t)byte_pos;
                                ctxt->VWheelSize = report_size;
                                break;
                            }
                        }
                        else if (usage_page == 0x0C)
                        {
                            if (pending_usages[u] == 0x0238)
                            {
                                // AC Pan
                                ctxt->HWheelIndex = (int16_t)byte_pos;
                                ctxt->HWheelSize = report_size;
                            }
                        }
                    }
                }
                bit_offset += report_size * report_count;
                pending_count = 0;
            }
        }
    }
}

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
    int bigEndian)
{
    uint32_t result = 0;
    uint8_t *buffer = (uint8_t *)src;
    if (bigEndian != 0)
    {
        for (int i = 0; i < n; i++)
        {
            result <<= 8;
            result |= (buffer[index + i] & 0xffL);
        }
    }
    else
    {
        for (int i = n - 1; i >= 0; i--)
        {
            result <<= 8;
            result |= (buffer[index + i] & 0xffL);
        }
    }
    return result;
}

/**
 *  @brief HID マウス ディスクリプタ マップ 値取得 @n
 *    値を取得する。
 *  @return 取得した値。取れない場合は0。
 */
static uint32_t get_value(
    int32_t index, int32_t size,
    const uint8_t *report, uint16_t len)
{
    uint32_t result = 0;

    if ((index >= 0) &&
        (size > 0))
    {
        // ビット数をバイト数に変換
        int byte_count = (size + 7) / 8;

        // バッファ境界チェック
        if (Decoders_CanDecode(index, byte_count, report, len))
        {
            // データを読み込む(リトルエンディアン)
            result = Decoders_nAt(index, byte_count, report, 0);
        }
    }

    return result;
}

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
    const HidMouseDescriptorMap *ctxt)
{
    uint32_t result = 0;

    if (ctxt != NULL)
    {
        result = get_value(
            ctxt->ButtonIndex, ctxt->ButtonSize,
            report, len);
    }

    return result;
}

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
    const HidMouseDescriptorMap *ctxt)
{
    int32_t result = 0;

    if (ctxt != NULL)
    {
        uint32_t v = get_value(
            ctxt->XIndex, ctxt->XSize,
            report, len);

        result = SignExtend(v, ctxt->XSize);
    }

    return result;
}

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
    const HidMouseDescriptorMap *ctxt)
{
    int32_t result = 0;

    if (ctxt != NULL)
    {
        uint32_t v = get_value(
            ctxt->YIndex, ctxt->YSize,
            report, len);

        result = SignExtend(v, ctxt->YSize);
    }

    return result;
}

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
    const HidMouseDescriptorMap *ctxt)
{
    int32_t result = 0;

    if (ctxt != NULL)
    {
        uint32_t v = get_value(
            ctxt->VWheelIndex, ctxt->VWheelSize,
            report, len);

        result = SignExtend(v, ctxt->VWheelSize);
    }

    return result;
}

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
    const HidMouseDescriptorMap *ctxt)
{
    int32_t result = 0;

    if (ctxt != NULL)
    {
        uint32_t v = get_value(
            ctxt->HWheelIndex, ctxt->HWheelSize,
            report, len);

        result = SignExtend(v, ctxt->HWheelSize);
    }

    return result;
}
