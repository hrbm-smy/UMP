/** -------------------------------------------------------------------------
 *
 *	@file	DebugUart.c
 *	@brief	Debug UART
 *	@author	H.Someya
 *	@date	2026/10/03
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
#include "DebugUart.h"

#if (DEBUGUART_ENABLED == 1)

#include "hardware/uart.h"
#include "hardware/gpio.h"

/* --------------------------------------------------------------------------
 *  P R I V A T E S
 */

#define DEBUG_UART_ID uart0
#define DEBUG_UART_BAUD 115200
#define DEBUG_UART_TX_PIN 16

/* --------------------------------------------------------------------------
 *  P U B L I C   I N T E R F A C E S
 */

/**
 *  @brief Debug UART 初期化 @n
 *    デバッグ用UARTを初期化する。
 *  @return なし。
 */
void DebugUart_Init(void)
{
    uart_init(DEBUG_UART_ID, DEBUG_UART_BAUD);
    gpio_set_function(DEBUG_UART_TX_PIN, GPIO_FUNC_UART);
}

/**
 *  @brief Debug UART メッセージ送信 @n
 *    メッセージを送信する。
 *    どれだけ待機が入るとかは初期バージョンでは調べていないので注意。
 *  @param message 送信するメッセージ。
 *  @return なし。
 */
void DebugUart_Puts(const char *message)
{
    uart_puts(DEBUG_UART_ID, message);
}

#endif // (DEBUGUART_ENABLED == 1)
