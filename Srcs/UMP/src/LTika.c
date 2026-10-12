/** -------------------------------------------------------------------------
 *
 *	@file	LTika.c
 *	@brief	LチカのLED
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
#include "LTika.h"

#include "hardware/pio.h"

/* --------------------------------------------------------------------------
 *  P R I V A T E S
 */

#define LED_PIN 25

/* --------------------------------------------------------------------------
 *  P U B L I C   I N T E R F A C E S
 */

/**
 *  @brief Lチカ初期化 @n
 *    Lチカできるように初期化する。
 *  @return なし。
 */
void LTika_Init(void)
{
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
}

/**
 *  @brief Lチカ点灯/消灯 @n
 *    点灯/消灯させる。
 *  @param state 非0=点灯、0=消灯。
 *  @return なし。
 */
void LTika_Set(int state)
{
    // 仕様は非0で「点灯」。
    // ハードウェアの特性はここで吸収する。
    // 今回はOFFで点灯。
    gpio_put(LED_PIN, (state == 0));
}
