/**
* @addtogroup 文件系统
* @{
* @defgroup presearch presearch
* @{
*/

//  maintainer: lilu
#ifndef __PRESEARCH_H__
#define __PRESEARCH_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

/**
 * @brief 文件系统初始化
 * @param DeviceID		设备号
 * @return 成功：TRUE 失败：FALSE
 */
bool FSInit(uint8_t DeviceID);

/**
 * @brief 文件系统去初始化
 * @param DeviceID		设备号
 * @return 成功：TRUE 失败：FALSE
 */
bool FSDeInit(uint8_t DeviceID);

extern bool IsFsInited(void);


#ifdef FUNC_SPI_UPDATE_EN
extern uint8_t UpgradeFileFound;
#ifdef  FUNC_UPDATE_CONTROL	
extern uint8_t ConfirmUpgradeFlag;
#endif
#endif// FUNC_SPI_UPDATE_EN

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif

/**
 * @}
 * @}
 */
