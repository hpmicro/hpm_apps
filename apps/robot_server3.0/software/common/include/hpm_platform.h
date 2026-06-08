/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __HPM_PLATFORM_H
#define __HPM_PLATFORM_H

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
int hpm_platform_flash_init(void);

/**
 * @brief get platform flash init status interface
 *        ;获取flash初始化状态
 *
 * @return flash init status ;falsh初始化状态，0: no init or init fail, 1: init success.
 */
bool hpm_platform_get_flash_initdone(void);

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
 * @brief Profile entry get interface
 *        ;获取配置文件入口
 *
 * @return physical address;配置文件入口对应物理地址
 */
unsigned long hpm_profile_entry(void);

/**
 * @brief Profile read interface
 *        ;配置文件读接口
 *
 * @param [in] addr profile read addr ;配置文件读地址
 * @param [in] buffer package array to read data ;数据读取buffer
 * @param [in] len read data length ;数据读取长度
 * @return actual read number;实际读取个数
 */
int hpm_profile_read(unsigned int addr, unsigned char* buffer, unsigned int len);

/**
 * @brief Profile write interface
 *        ;配置文件写接口
 *
 * @param [in] addr profile write addr ;配置文件写地址
 * @param [in] buffer package array to write data ;数据写buffer
 * @param [in] len write data length ;数据写入长度
 * @return actual write number;实际写入个数
 */
int hpm_profile_write(unsigned int addr, const unsigned char* buffer, unsigned int len);

/**
 * @brief Profile cleanup interface (used for airconfig)
 *        ;配置文件清除接口
 *
 * @return clear status;清除状态，0: success, -1: fail.
 */
int hpm_profile_cleanup(void);

/**
 * @brief Get PSM area space size interface (file or flash).
 *        ;获取PSM区size（字节）
 *
 * @param [in] psm_type psm type ;psm类型 
 * @return psm size;psm所占字节大小
 */
int hpm_psm_size(unsigned int psm_type);

/**
 * @brief PSM area raw write interface (file or flash).
 *        ;PSM区写接口
 *
 * @param [in] psm_type psm type ;psm类型
 * @param [in] addr psm write addr ;psm写入地址
 * @param [in] buffer package array to write data ;数据写buffer
 * @param [in] len write data length ;数据写入长度 
 * @return actual write number;实际写入个数
 */
int hpm_psm_write(unsigned int psm_type, unsigned int addr,
        const unsigned char* buffer, unsigned int len);

/**
 * @brief PSM area raw erase interface (file or flash, set 0xFF...0xFF).
 *        ;PSM区擦除接口
 *
 * @param [in] psm_type psm type ;psm类型
 * @param [in] addr psm erase addr ;psm擦除地址
 * @param [in] len erase data length ;擦除数据长度 
 * @return actual erase number;实际擦除个数
 */
int hpm_psm_erase(unsigned int psm_type, unsigned int addr, unsigned int len);

/**
 * @brief PSM area raw read interface (file or flash).
 *        ;PSM区写接口
 *
 * @param [in] psm_type psm type ;psm类型
 * @param [in] addr psm read addr ;psm读地址
 * @param [in] buffer package array to read data ;数据读buffer
 * @param [in] len read data length ;数据读取长度 
 * @return actual read number;实际读取个数
 */
int hpm_psm_read(unsigned int psm_type, unsigned int addr,
        unsigned char* buffer, unsigned int len);

/**
 * @brief file system read interface
 *        ;系统文件读取
 *
 * @param [in] addr file system read addr ;系统文件读取地址
 * @param [in] buffer package array to read data ;数据读buffer
 * @param [in] len read data length ;数据读取长度 
 * @return actual read number;实际读取个数
 */
int hpm_filesystem_read(unsigned int addr, unsigned char* buffer, unsigned int len);

/**
 * @brief file system write interface
 *        ;系统文件写入
 *
 * @param [in] addr file system write addr ;系统文件写入地址
 * @param [in] buffer package array to write data ;数据写buffer
 * @param [in] len write data length ;数据写入长度 
 * @return actual write number;实际写入个数
 */
int hpm_filesystem_write(unsigned int addr, const unsigned char* buffer, unsigned int len);

/**
 * @brief file system auto write interface, auto erase and write
 *        ;系统文件自动写入，自动擦除接口
 *
 * @param [in] addr file system write addr ;系统文件写入地址
 * @param [in] buffer package array to write data ;数据写buffer
 * @param [in] len write data length ;数据写入长度 
 * @return actual write number;实际写入个数
 */
int hpm_filesystem_auto_write(unsigned int addr, const unsigned char* buffer, unsigned int len);

/**
 * @brief filesystem erase interface (file or flash, set 0xFF...0xFF).
 *        ;系统文件擦除接口
 *
 * @param [in] addr file system erase addr ;系统文件擦除地址
 * @param [in] len erase data length ;数据擦除长度 
 * @return actual erase number;实际擦除个数
 */
int hpm_filesystem_erase(unsigned int addr, unsigned int len);

/**
 * @brief file system cleanup interface
 *        ;系统文件清除接口
 *
 * @return clear status;清除状态，0: success, -1: fail.
 */
int hpm_filesystem_cleanup(void);

/**
 * @brief userconmmon read interface
 *        ;usercommon区读接口
 *
 * @param [in] addr userconmmon read addr ;userconmmon区读取地址
 * @param [in] buffer package array to read data ;数据读buffer
 * @param [in] len read data length ;数据读取长度 
 * @return actual read number;实际读取个数
 */
int hpm_usercommon_read(unsigned int addr, unsigned char* buffer, unsigned int len);

/**
 * @brief userconmmon write interface
 *        ;usercommon区写接口
 *
 * @param [in] addr userconmmon write addr ;userconmmon区写入地址
 * @param [in] buffer package array to write data ;数据写buffer
 * @param [in] len write data length ;数据写入长度 
 * @return actual write number;实际写入个数
 */
int hpm_usercommon_write(unsigned int addr, const unsigned char* buffer, unsigned int len);

/**
 * @brief userconmmon erase interface (file or flash, set 0xFF...0xFF).
 *        ;userconmmon区擦除接口
 *
 * @param [in] addr userconmmon erase addr ;userconmmon区擦除地址
 * @param [in] len erase data length ;数据擦除长度 
 * @return actual erase number;实际擦除个数
 */
int hpm_usercommon_erase(unsigned int addr, unsigned int len);

/**
 * @brief usercommon cleanup interface
 *        ;usercommon区清除接口
 *
 * @return clear status;清除状态，0: success, -1: fail.
 */
int hpm_usercommon_cleanup(void);

#ifdef __cplusplus
}
#endif

/** @} */

#endif //__HPM_PLATFORM_H

