/** -------------------------------------------------------------------------
 *
 *	@file	main.c
 *	@brief	USB Mouse Proxy
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
#include <string.h>
#include "pico/multicore.h"

#include "LTika.h"
#include "DebugUart.h"

#include "UmpConfig.h"
#include "UmpHost.h"
#include "UmpDevice.h"

/* --------------------------------------------------------------------------
 *  Core 0–Core 1 Interfaces
 */

/* --------------------------------------------------------------------------
 *  Core 1 - PIO USB Host
 */

static void Core1Main(void)
{
    while (true)
    {
        UmpHost_Drive(UmpHost_Instance());
    }
}

/* --------------------------------------------------------------------------
 *  Core 0 - Leader, USB Device
 */

static void Core0Main(void)
{
    UmpHost *umpHost = UmpHost_Instance();
    while (true)
    {
        // Patchbay
        UmpDevice_Drive_In uddIn;
        memset(&uddIn, 0, sizeof uddIn);
        uddIn.Config = UmpConfig_Get();
        uddIn.DescRing = &umpHost->DescRing;
        uddIn.ReportRing = &umpHost->ReportRing;
        UmpDevice_Drive(&uddIn, UmpDevice_Instance());
    }
}

int main(void)
{
    // 各モジュールに先立って、ミドルウェア(的なもの)を初期化
    LTika_Init();
    DEBUGUART_INIT();
    DEBUGUART_PUTS("[BOOT] Hello, World!\r\n");

    // 各モジュールを初期化
    UmpHost_Init(UmpHost_Instance());
    UmpDevice_Init(UmpDevice_Instance());

    // Core1を起動
    multicore_launch_core1(Core1Main);
    DEBUGUART_PUTS("[BOOT] Core1 launched\r\n");

    // Core0を起動
    Core0Main();

    // Core0Main は抜けてこないのだがお約束
    for (;;)
    {
    }
}
