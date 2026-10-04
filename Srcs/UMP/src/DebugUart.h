#ifndef __DebugUart_H__
#define __DebugUart_H__

/** -------------------------------------------------------------------------
 *
 *	@file	DebugUart.h
 *	@brief	Debug UART
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

/* --------------------------------------------------------------------------
 *  P U B L I C   D E F I N I T I O N S
 */

/**
 * Debug UART 有効/無効
 * 1=有効
 */
// #define DEBUGUART_ENABLED (1)

/* --------------------------------------------------------------------------
 *  P U B L I C   I N T E R F A C E S
 */

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     *  @brief Debug UART 初期化 @n
     *    デバッグ用UARTを初期化する。
     *  @return なし。
     */
    void DebugUart_Init(void);

#if (DEBUGUART_ENABLED == 1)
#define DEBUGUART_INIT() DebugUart_Init()
#else
#define DEBUGUART_INIT()
#endif

    /**
     *  @brief Debug UART メッセージ送信 @n
     *    メッセージを送信する。
     *    どれだけ待機が入るとかは初期バージョンでは調べていないので注意。
     *  @param message 送信するメッセージ。
     *  @return なし。
     */
    void DebugUart_Puts(const char *message);

#if (DEBUGUART_ENABLED == 1)
#define DEBUGUART_PUTS(s) DebugUart_Puts(s)
#else
#define DEBUGUART_PUTS(s)
#endif

#ifdef __cplusplus
}
#endif

#endif
