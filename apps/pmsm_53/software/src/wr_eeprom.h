#ifndef EEPROM_H_
#define EEPROM_H_

#include <stdio.h>
#include <assert.h>
#include "board.h"
#include "hpm_sysctl_drv.h"
#include "hpm_i2c_drv.h"
#include "hpm_i2c_regs.h"
#include "hpm_gpio_drv.h"
#include "parm_global.h"

/**
 * @brief     eeprom pins config
 *            ;eeprom 引脚配置
 */
void iic_pin_cfg(void);

/**
 * @brief     param save to eeprom
 *            ;参数保存至eeprom
 */
void parmStoreAll(void);

/**
 * @brief     params load from eeprom
 *            ;从eeprom加载参数
 * @param[in]    address  read address;加载数据存放地址
 */
void parmReadAll(uint8_t* address);

#endif//EEPROM_H_