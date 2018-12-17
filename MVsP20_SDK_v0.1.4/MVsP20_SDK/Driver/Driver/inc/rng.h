/**
 *******************************************************************************
 * @file    rng.h
 * @author  
 * @version V2.0.0
 * @date    2014/09/22
 *
 *******************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, MVSILICON SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * <h2><center>&copy; COPYRIGHT 2014 MVSilicon </center></h2>
 */

/**
 * @addtogroup TIMER
 * @{
 * @defgroup rng rng.h
 * @{
 */

#ifndef __RNG_H__
#define __RNG_H__
#ifdef __cplusplus
extern "C" {
#endif//__cplusplus


/**
 * @brief     获取随机数
 *
 * @param 无
 * @return  随机数
 */
uint32_t RNG_GetRandomSeed(void);

#ifdef  __cplusplus
}
#endif//__cplusplus

/**
 * @}
 * @}
 */
