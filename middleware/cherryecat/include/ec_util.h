/*
 * Copyright (c) 2025, sakumisu
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EC_UTIL_H
#define EC_UTIL_H

#if defined(__CC_ARM)
#ifndef __USED
#define __USED __attribute__((used))
#endif
#ifndef __WEAK
#define __WEAK __attribute__((weak))
#endif
#ifndef __PACKED
#define __PACKED __attribute__((packed))
#endif
#ifndef __PACKED_STRUCT
#define __PACKED_STRUCT __packed struct
#endif
#ifndef __PACKED_UNION
#define __PACKED_UNION __packed union
#endif
#ifndef __ALIGNED
#define __ALIGNED(x) __attribute__((aligned(x)))
#endif
#elif defined(__GNUC__)
#ifndef __USED
#define __USED __attribute__((used))
#endif
#ifndef __WEAK
#define __WEAK __attribute__((weak))
#endif
#ifndef __PACKED
#define __PACKED __attribute__((packed, aligned(1)))
#endif
#ifndef __PACKED_STRUCT
#define __PACKED_STRUCT struct __attribute__((packed, aligned(1)))
#endif
#ifndef __PACKED_UNION
#define __PACKED_UNION union __attribute__((packed, aligned(1)))
#endif
#ifndef __ALIGNED
#define __ALIGNED(x) __attribute__((aligned(x)))
#endif
#elif defined(__ICCARM__) || defined(__ICCRX__) || defined(__ICCRISCV__)
#if (__VER__ >= 8000000)
#define __ICCARM_V8 1
#else
#define __ICCARM_V8 0
#endif

#ifndef __USED
#if defined(__ICCARM_V8) || defined(__ICCRISCV__)
#define __USED __attribute__((used))
#else
#define __USED __root
#endif
#endif

#ifndef __WEAK
#if defined(__ICCARM_V8) || defined(__ICCRISCV__)
#define __WEAK __attribute__((weak))
#else
#define __WEAK _Pragma("__weak")
#endif
#endif

#ifndef __PACKED
#if defined(__ICCARM_V8) || defined(__ICCRISCV__)
#define __PACKED __attribute__((packed, aligned(1)))
#else
/* Needs IAR language extensions */
#define __PACKED __packed
#endif
#endif

#ifndef __PACKED_STRUCT
#if defined(__ICCARM_V8) || defined(__ICCRISCV__)
#define __PACKED_STRUCT struct __attribute__((packed, aligned(1)))
#else
/* Needs IAR language extensions */
#define __PACKED_STRUCT __packed struct
#endif
#endif

#ifndef __PACKED_UNION
#if defined(__ICCARM_V8) || defined(__ICCRISCV__)
#define __PACKED_UNION union __attribute__((packed, aligned(1)))
#else
/* Needs IAR language extensions */
#define __PACKED_UNION __packed union
#endif
#endif

#ifndef __ALIGNED
#if defined(__ICCARM_V8) || defined(__ICCRISCV__)
#define __ALIGNED(x) __attribute__((aligned(x)))
#elif (__VER__ >= 7080000)
/* Needs IAR language extensions */
#define __ALIGNED(x) __attribute__((aligned(x)))
#else
#warning No compiler specific solution for __ALIGNED.__ALIGNED is ignored.
#define __ALIGNED(x)
#endif
#endif

#endif

#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

#define EC_ALIGN_UP(size, align) (((size) + (align)-1) & ~((align)-1))

#define EC_WRITE_U8(DATA, VAL)                   \
    do {                                         \
        *((uint8_t *)(DATA)) = ((uint8_t)(VAL)); \
    } while (0)

#define EC_WRITE_U16(DATA, VAL)                    \
    do {                                           \
        *((uint16_t *)(DATA)) = ((uint16_t)(VAL)); \
    } while (0)

#define EC_WRITE_U32(DATA, VAL)                    \
    do {                                           \
        *((uint32_t *)(DATA)) = ((uint32_t)(VAL)); \
    } while (0)

#define EC_WRITE_U64(DATA, VAL)                    \
    do {                                           \
        *((uint64_t *)(DATA)) = ((uint64_t)(VAL)); \
    } while (0)

#define EC_READ_U8(DATA) \
    ((uint8_t) * ((uint8_t *)(DATA)))

#define EC_READ_U16(DATA) \
    ((uint16_t) * ((uint16_t *)(DATA)))

#define EC_READ_U32(DATA) \
    ((uint32_t) * ((uint32_t *)(DATA)))

#define EC_READ_U64(DATA) \
    ((uint64_t) * ((uint64_t *)(DATA)))

#define ec_htons(A) ((((uint16_t)(A)&0xff00) >> 8) | \
                     (((uint16_t)(A)&0x00ff) << 8))
#define ec_htonl(A) ((((uint32_t)(A)&0xff000000) >> 24) | \
                     (((uint32_t)(A)&0x00ff0000) >> 8) |  \
                     (((uint32_t)(A)&0x0000ff00) << 8) |  \
                     (((uint32_t)(A)&0x000000ff) << 24))

#endif