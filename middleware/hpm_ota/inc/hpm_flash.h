/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __HPM_PLASH_H
#define __HPM_PLASH_H

#include <stdbool.h>

/**
 * @brief Hpm platform API;platform接口
 * @addtogroup Hpm_platform_API
 * @{
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief platform flash init interface
 *        ;flash初始化
 *
 * @return flash init status ;falsh初始化状态，0: init fail, 1: init success.
 */
int hpm_flash_init(void);

/**
 * @brief get platform flash init status interface
 *        ;获取flash初始化状态
 *
 * @return flash init status ;falsh初始化状态，0: no init or init fail, 1: init success.
 */
bool hpm_get_flash_initdone(void);

/**
 * @brief flash read interface
 *        ;从flash读接口
 * 
 * @param [in] addr read flash addr ;flash读地址
 * @param [in] buffer package array to read data ;数据读取buffer
 * @param [in] len read data length ;读取数据长度
 * @return actual read number;实际读取到的个数
 */
int hpm_flash_read(unsigned int addr, unsigned char* buffer, unsigned int len);

int hpm_flash_localmem_read(unsigned int addr, unsigned char* buffer, unsigned int len);

/**
 * @brief flash write interface
 *        ;向flash写接口
 * 
 * @param [in] addr write flash addr ;flash写地址
 * @param [in] buffer package array to write data ;数据写入buffer
 * @param [in] len write data length ;写入数据长度
 * @return actual write number;实际写入个数
 */
int hpm_flash_write(unsigned int addr, const unsigned char* buffer, unsigned int len);

/**
 * @brief flash sector erase interface
 *        ;flash切片擦除接口
 * 
 * @param [in] addr flash sector erase addr ;flash切片擦除地址
 * @return actual erase number;实际擦除个数
 */
int hpm_flash_erase_sector(unsigned int addr);

/**
 * @brief flash erase interface
 *        ;flash擦除接口
 * 
 * @param [in] addr flash erase addr ;flash擦除地址
 * @param [in] len flash erase len ;flash擦除长度
 * @return actual erase number;实际擦除个数
 */
int hpm_flash_erase(unsigned int addr, unsigned int len);

/**
 * @brief check flash is erased
 *        ; 判断flash 指定地址及长度是否已被擦除
 * 
 * @param [in] addr flash erase addr; flash 擦除地址
 * @param [in] len flash erase len; flash 擦除长度
 * @return bool, true is erased, false no erase
 */
bool hpm_flash_is_erased(unsigned int addr, unsigned int len);

/**
 * @brief flash erase and rewrite interface
 *        ; flash擦除指定地址和长度接口，由于flash 特性，需按sector擦除，此接口会按照传入的地址和长度擦除
 * 
 * @param [in] addr flash erase addr; flash 擦除地址
 * @param [in] len flash erase len; flash 擦除长度
 * @return int actual erase num; 实际擦除长度
 */
int hpm_flash_erase_and_rewrite(unsigned int addr, unsigned int len);

/**
 * @brief flash empty erase mask
 *        ;flash 清空 erase mask
 */
void hpm_flash_empty_erase_mask(void);

/**
 * @brief flash auto write, According to mask, erase before write
 *        ;flash自动写，根据mask，先擦后写
 * 
 * @param [in] addr write flash addr ;flash写地址
 * @param [in] buffer package array to write data ;数据写入buffer
 * @param [in] len write data length ;写入数据长度
 * @return actual write number;实际写入个数
 */
int hpm_flash_erase_write_of_mask(unsigned int addr, const unsigned char *buffer, unsigned int len);

#ifdef __cplusplus
}
#endif

/** @} */

#endif //__HPM_PLASH_H

