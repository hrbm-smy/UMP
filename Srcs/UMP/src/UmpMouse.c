/** -------------------------------------------------------------------------
 *
 *	@file	UmpMouse.c
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
#include "UmpMouse.h"

#include <stddef.h>
#include <string.h>

/* --------------------------------------------------------------------------
 *  P R I V A T E S
 */

/* --------------------------------------------------------------------------
 *  P U B L I C   I N T E R F A C E S
 */

/**
 *  @brief UMPマウスコンテキスト 初期化 @n
 *    このモジュールを初期化する。
 *  @param ctxt コンテキスト。
 *  @return なし。
 */
void UmpMouse_Init(
    UmpMouse *ctxt)
{
    if (ctxt != NULL)
    {
        memset(ctxt, 0, sizeof(UmpMouse));

        HidMouseDescriptorMap_Init(&ctxt->DescMap);

        RingedFrames_Init(
            UMPMOUSE_REPORTHIST_CAP,
            sizeof(HidReportPacket),
            ctxt->ReportHistBuffer, &ctxt->ReportHistRing);
    }
}
