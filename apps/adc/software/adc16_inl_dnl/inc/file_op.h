/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef FILE_OP_H
#define FILE_OP_H

/**
 * @brief Fils operation interface;文件操作接口
 * @defgroup Files_API
 * @ingroup Files_API 
 * @{
 *
 */

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Files scan in the specified path;对某个路径下进行文件扫描
 *
 * @param [in] path Path for files scan;扫描的路径
 */
void f_scan(char *path);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

/** @} */

#endif  /* FILE_OP_H */