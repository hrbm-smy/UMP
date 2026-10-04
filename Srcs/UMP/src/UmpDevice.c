/** -------------------------------------------------------------------------
 *
 *	@file	UmpDevice.c
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
#include "UmpDevice.h"

#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h> // Needed for pio_usb.h

#include "nullptr.h"
#include "Decoders.h"
#include "MinMax.h"

#include "pio_usb.h"
#include "tusb.h"

/* --------------------------------------------------------------------------
 *  P R I V A T E S
 */

/**
 * Key作成
 * @param address アドレス
 * @param instance インスタンス
 */
#define UMPDEVICE_KEY(address, instance) \
    ((((instance) << 8) & 0xff) |        \
     (((address) << 0) & 0xff))

/**
 * インスタンス。
 */
static UmpDevice _instance;

/**
 * @name 動作モード。
 *
 * 動作モード定義。
 *
 * @{
 */

/**
 * トラックスクロールモード。非0とすること。
 */
#define UMPDEVICE_MODE_TRACKSCROLL (1)

/** @}*/

#define BUTTON_X1_MASK (0x08)

#define UMPDEVICE_TRACK_SCROLL_THRESHOLD (16)

/* --------------------------------------------------------------------------
 *  P U B L I C   I N T E R F A C E S
 */

/**
 *  @brief USBマウスプロキシデバイス インスタンス取得 @n
 *    インスタンスを取得する。
 *  @return インスタンス。
 *    現状、TinyUSBの仕様(コールバックの仕様)から、複数インスタンスは動かせない。
 *    このインスタンスを使用すること。
 */
UmpDevice *UmpDevice_Instance(void)
{
    return &_instance;
}

/**
 *  @brief USBマウスプロキシデバイス 初期化 @n
 *    このモジュールを初期化する。
 *  @param ctxt コンテキスト。
 *    現状、TinyUSBの仕様(コールバックの仕様)から、複数インスタンスは動かせない。
 *    UmpDevice_Instance()を使用すること。
 *  @return なし。
 */
void UmpDevice_Init(
    UmpDevice *ctxt)
{
    if (ctxt != NULL)
    {
        memset(ctxt, 0, sizeof(UmpDevice));

        Map_Init(
            UMPDEVICE_DESC_CAP, ctxt->DescMapElms,
            &ctxt->DescMap);

        RingedFrames_Init(
            UMPDEVICE_REPORTHIST_CAP,
            sizeof(HidReportPacket),
            ctxt->ReportHistBuffer, &ctxt->ReportHistRing);

        // ハードウェアUSBコントローラでHIDマウスデバイス初期化
        tud_init(0);
    }
}

static void UmpDevice_Descriptor(
    const UmpDevice_Drive_In *inputs,
    UmpDevice *ctxt)
{
    // レポートディスクリプタをPop
    HidReportDescriptorPacket descPack;
    int32_t descLen = HidReportDescriptorRing_Pop(&descPack, inputs->DescRing);
    if (descLen > 0)
    {
        // Keyを生成し、対応するマウスマップを取得
        int32_t key = UMPDEVICE_KEY(descPack.Address, descPack.Instance);
        HidMouseDescriptorMap *mouseMap = Map_ValueFor(key, &ctxt->DescMap);

        // 無い場合には確保して対応づけ
        int32_t descMapCount = Map_Count(&ctxt->DescMap);
        int32_t descMapCap = sizeof(ctxt->MouseDescs);
        if ((mouseMap == nullptr) &&
            (descMapCount < descMapCap))
        {
            mouseMap = &ctxt->MouseDescs[descMapCount];
            Map_Relate(mouseMap, key, &ctxt->DescMap);
        }

        // 対応づけられた場合は更新
        if (mouseMap != NULL)
        {
            HidMouseDescriptorMap_Init(mouseMap);
            HidMouseDescriptorMap_Parse(
                descPack.ReportDesc, descPack.DescLength, mouseMap);
        }
    }
}

static int ShouldEnterTrackScrollMode(
    uint32_t button,
    int32_t currentMode,
    const UmpConfig *config)
{
    (void)config;

    int result = 0;

    if ((currentMode == 0) &&
        (button & BUTTON_X1_MASK))
    {
        // いまんとこX1でトラックスクロールモードに入る
        result = 1;
    }

    return result;
}

static int ShouldExitTrackScrollMode(
    uint32_t button,
    int32_t currentMode,
    const UmpConfig *config)
{
    (void)config;

    int result = 0;

    if ((currentMode != 0) &&
        (button != 0))
    {
        // なんか押されりゃ戻る
        result = 1;
    }

    return result;
}

static int8_t clamp_i32_to_i8(int32_t v)
{
    if (v > 127)
        return 127;
    if (v < -127)
        return -127;
    return (int8_t)v;
}

static int8_t consume_scroll_steps(
    int32_t *acc, int32_t delta, int32_t threshold)
{
    *acc += delta;

    int16_t steps = 0;
    // 割り算とあまり……なんだが、ここはCPU性能にかんがみて
    while (*acc >= threshold)
    {
        *acc -= threshold;
        steps++;
    }
    while (*acc <= -threshold)
    {
        *acc += threshold;
        steps--;
    }

    return clamp_i32_to_i8(steps);
}

static void UmpDevice_ExtendedReport(
    const HidReportPacket *reportPack,
    int32_t reportLen,
    const HidMouseDescriptorMap *mouseMap,
    const UmpDevice_Drive_In *inputs,
    UmpDevice *ctxt)
{
    // ともあれ履歴にPushしなおす
    RingedFrames_Push(
        reportPack, reportLen,
        0LL, &ctxt->ReportHistRing);

    // ボタンデータ取得
    uint32_t button = HidMouseDescriptorMap_Buttons(
        reportPack->Report, reportLen, mouseMap);

    // ボタンが「押された」「放された」エッジを検知
    uint32_t lastButton = ctxt->ButtonState;
    uint32_t buttonPressed = ctxt->ButtonState | button;
    // uint32_t buttonReleased = ~(ctxt->ButtonState & button);
    ctxt->ButtonState = button;

    // トラックスクロールON/OFF判定、モード遷移
    // これは「押下」エッジで遷移
    int32_t lastMode = ctxt->Mode;
    if (ShouldEnterTrackScrollMode(buttonPressed, ctxt->Mode, NULL))
    {
        ctxt->Mode = UMPDEVICE_MODE_TRACKSCROLL;
    }
    else if (ShouldExitTrackScrollMode(buttonPressed, ctxt->Mode, NULL))
    {
        ctxt->Mode = 0;
    }

    if ((ctxt->Mode != lastMode) &&
        (ctxt->ButtonState != lastButton))
    {
        // モードが変わったときのボタン押下/解放は転送しない
    }
    else
    {
        if (ctxt->Mode == UMPDEVICE_MODE_TRACKSCROLL)
        {
            // 縦横どちらにスクロールするか、履歴をたどって傾向を調べる。
            // ジェスチャ認識、的アプローチ。

            // X、Yの累積移動を計算する
            // ここでは、例えば、
            // 「左右にふらふらしながらも上に移動した」なんていう場合、「縦である」
            // という認識にするため、符号付きで累積する。
            int32_t xAcc = 0;
            int32_t yAcc = 0;
            int32_t histCnt = MinS32(
                UMPDEVICE_REPORTHIST_CAP,
                RingedFrames_Count(&ctxt->ReportHistRing));
            for (int i = 0; i < histCnt; i++)
            {
                // 最新のリポートを取得
                const HidReportPacket *curr = RingedFrames_ReferWithNew(
                    i, NULL, NULL, &ctxt->ReportHistRing);

                int32_t hx = HidMouseDescriptorMap_X(
                    curr->Report, curr->ReportLength, mouseMap);
                int32_t hy = HidMouseDescriptorMap_Y(
                    curr->Report, curr->ReportLength, mouseMap);

                xAcc += hx;
                yAcc += hy;
            }

            // ただし、最終的に判断するにおいて、絶対値で判定する。
            // これで、「傾向」になる。
            // ハズ。
            int dir = 0;
            if (abs(yAcc) > abs(xAcc))
            {
                dir = 1;
            }

            // 最終的にレポートをまとめていくよ。
            uint8_t report[5];
            memset(report, 0, sizeof report);

            // ボタンはそのまま
            report[0] = button;
            // Xは0
            report[1] = 0;
            // Yも0
            report[2] = 0;

            // で、ここでやっとどれだけスクロールするか。
            // ここでは傾向関係ない。
            if (dir == 0)
            {
                int32_t x = HidMouseDescriptorMap_X(
                    reportPack->Report, reportLen, mouseMap);
                int8_t steps = consume_scroll_steps(
                    &ctxt->ScrollAcc, x,
                    UMPDEVICE_TRACK_SCROLL_THRESHOLD);
                report[3] = steps;
            }
            else
            {
                int32_t y = HidMouseDescriptorMap_Y(
                    reportPack->Report, reportLen, mouseMap);
                int8_t steps = consume_scroll_steps(
                    &ctxt->ScrollAcc, y,
                    UMPDEVICE_TRACK_SCROLL_THRESHOLD);
                report[4] = steps;
            }

            // で、レポート送信。
            tud_hid_report(1, report, sizeof report);
        }
        else
        {
            // 通常モード(ないしはモード不明)時は単に転送
            uint8_t report[5];
            memset(report, 0, sizeof report);

            report[0] = (uint8_t)button;
            report[1] = HidMouseDescriptorMap_X(
                reportPack->Report, reportLen, mouseMap);
            report[2] = HidMouseDescriptorMap_Y(
                reportPack->Report, reportLen, mouseMap);
            report[3] = HidMouseDescriptorMap_VWheel(
                reportPack->Report, reportLen, mouseMap);
            report[4] = HidMouseDescriptorMap_HWheel(
                reportPack->Report, reportLen, mouseMap);

            tud_hid_report(1, report, sizeof report);
        }
    }
}

static void UmpDevice_Report(
    const UmpDevice_Drive_In *inputs,
    UmpDevice *ctxt)
{
    // レポートをPop
    HidReportPacket reportPack;
    int32_t reportLen = HidReportRing_Pop(&reportPack, inputs->ReportRing);
    if (reportLen > 0)
    {
        // Keyを生成し、対応するマウスマップを取得
        int32_t key = UMPDEVICE_KEY(reportPack.Address, reportPack.Instance);
        HidMouseDescriptorMap *mouseMap = Map_ValueFor(key, &ctxt->DescMap);

        if (mouseMap == NULL)
        {
            // 対応づけられない場合はデフォルト動作
            uint8_t report[5];
            memset(report, 0, sizeof report);
            report[0] = Decoders_Defaulted8At(
                0, reportPack.Report, reportPack.ReportLength, 0);
            report[1] = Decoders_Defaulted8At(
                1, reportPack.Report, reportPack.ReportLength, 0);
            report[2] = Decoders_Defaulted8At(
                2, reportPack.Report, reportPack.ReportLength, 0);

            // レポート送信
            tud_hid_report(1, report, sizeof report);
        }
        else
        {
            // 対応づけられた場合、いろいろ工夫が始まる
            UmpDevice_ExtendedReport(
                &reportPack, reportLen,
                mouseMap,
                inputs, ctxt);
        }
    }
}

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
    UmpDevice *ctxt)
{
    if ((ctxt != NULL) &&
        (inputs != NULL))
    {
        tud_task();

        if (tud_hid_ready())
        {
            // レポートディスクリプタの処理
            UmpDevice_Descriptor(inputs, ctxt);

            // レポートの処理
            UmpDevice_Report(inputs, ctxt);
        }
    }
}
