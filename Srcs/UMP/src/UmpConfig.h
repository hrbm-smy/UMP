#ifndef __UmpConfig_H__
#define __UmpConfig_H__

/** -------------------------------------------------------------------------
 *
 *	@file	UmpConfig.h
 *	@brief	USB Mouse Proxy Configuration
 *	@author	H.Someya
 *	@date	2026/10/05
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
     * USB Mouse Proxy Configuration
     * 将来に期待。
     */
    typedef struct _UmpConfig
    {
        /**
         * トラックスクロールしきい値。
         */
        int32_t TrackScrollThreshold;

        /**
         * 水平(横)スクロールシュミットトリガしきい値。
         * X方向の移動量がこのパラメータで指定した%以上になると
         * 水平スクロールに切り替わる。
         */
        int32_t HSchmittPercentage;
        /**
         * 垂直(縦)スクロールシュミットトリガしきい値。
         * X方向の移動量がこのパラメータで指定した%未満になると
         * 垂直スクロールに切り替わる。
         */
        int32_t VSchmittPercentage;
    } UmpConfig;

    /**
     *  @brief UMPコンフィグレーション取得 @n
     *    UMPコンフィグレーションを取得する。
     *  @return UMPコンフィグレーション。
     */
    const UmpConfig *UmpConfig_Get(void);

#ifdef __cplusplus
}
#endif

#endif
