/** -------------------------------------------------------------------------
 *
 *	@file	RawLock.c
 *	@brief	Low-level mutual exclusion lock.
 *	@author	H.Someya
 *	@date	2026/09/27
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
#include "RawLock.h"

#include <stddef.h>
#include <string.h>
#include "hardware/sync.h"

/* --------------------------------------------------------------------------
 *  P R I V A T E S
 */

/* --------------------------------------------------------------------------
 *  P U B L I C   I N T E R F A C E S
 */

/**
 *  @brief 低レベル排他制御 初期化 @n
 *    低レベル排他制御を初期化する。
 *  @param ctxt コンテキスト。
 *  @return 非0=成功、0=失敗。
 */
int RawLock_Init(RawLock *ctxt)
{
	int result = 0;

	if (ctxt != NULL)
	{
		// コンテキストをクリア
		memset(ctxt, 0, sizeof(RawLock));

		// 未使用のハードウェアスピンロックを要求
		int id = spin_lock_claim_unused(false);
		if (id >= 0)
		{
			// 取れたら初期化、結果は成功
			ctxt->Handle = spin_lock_init((uint)id);
			result = 1;
		}
	}

	return result;
}

/**
 *  @brief 低レベル排他制御 ロック @n
 *    ロックする。
 *  @param ctxt コンテキスト。
 *  @return 非0=成功、0=失敗。
 */
int RawLock_Lock(RawLock *ctxt)
{
	int result = 0;

	if ((ctxt != NULL) &&
		(ctxt->Handle != NULL))
	{
		ctxt->State = spin_lock_blocking(ctxt->Handle);
		result = 1;
	}

	return result;
}

/**
 *  @brief 低レベル排他制御 アンロック @n
 *    アンロックする。
 *  @param ctxt コンテキスト。
 *  @return なし。
 */
void RawLock_Unlock(RawLock *ctxt)
{
	if ((ctxt != NULL) &&
		(ctxt->Handle != NULL))
	{
		spin_unlock(ctxt->Handle, ctxt->State);
	}
}
