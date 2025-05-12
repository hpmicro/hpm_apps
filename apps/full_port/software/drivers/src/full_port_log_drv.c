/**
 * Copyright (c) 2021-2024 HPMicro
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#include "full_port_config_drv.h"

void full_port_log_stash_info(int16_t s16Level, char* p_s8Info, ...)
{
    uint8_t p_u8LogBuf[256];
    int32_t s32Index;

    switch (s16Level)
    {
    case FULL_PORT_LOG_LEVEL_DEBUG:
        memcpy(p_u8LogBuf, "[D]", 3);
        break;
    case FULL_PORT_LOG_LEVEL_INFO:
        memcpy(p_u8LogBuf, "[I]", 3);
        break;
    case FULL_PORT_LOG_LEVEL_WARNING:
        memcpy(p_u8LogBuf, "[W]", 3);
        break;
    case FULL_PORT_LOG_LEVEL_ERROR:
        memcpy(p_u8LogBuf, "[E]", 3);
        break;
    default:
        break;
    }

    va_list args;
    va_start(args, p_s8Info);
    s32Index = vsnprintf((char*)p_u8LogBuf+3, 250, (const char*)p_s8Info, args);
    va_end(args);
    s32Index = ((s32Index > 250) | (s32Index < 0)) ? \
                250 : s32Index;
    if (250 > s32Index) {
        p_u8LogBuf[s32Index+3] = '\0';
        printf("%s",p_u8LogBuf);        
    } else {
        printf("%s %s %d error!\r\n",__FILE__, __func__, __LINE__);
    }

}

void full_port_log_stash_array(int16_t s16Level, uint8_t* p_u8Array, uint16_t u16ArrayLenth, char* p_s8Info, ...)
{
    uint8_t p_u8LogBuf[256];
    int32_t s32Index;
    uint16_t u16TransCnt;

    switch (s16Level)
    {
    case FULL_PORT_LOG_LEVEL_DEBUG:
        memcpy(p_u8LogBuf, "[D]", 3);
        break;
    case FULL_PORT_LOG_LEVEL_INFO:
        memcpy(p_u8LogBuf, "[I]", 3);
        break;
    case FULL_PORT_LOG_LEVEL_WARNING:
        memcpy(p_u8LogBuf, "[W]", 3);
        break;
    case FULL_PORT_LOG_LEVEL_ERROR:
        memcpy(p_u8LogBuf, "[E]", 3);
        break;
    default:
        break;
    }

    va_list args;
    va_start(args, p_s8Info);
    s32Index = vsnprintf((char*)p_u8LogBuf+3, 250, (const char*)p_s8Info, args);
    va_end(args);

    s32Index = ((s32Index > 250) | (s32Index < 0)) ? 250 : s32Index;
    if (250 > s32Index)
    {
        u16TransCnt = (u16ArrayLenth > ((250-s32Index)/3)) ? ((250-s32Index)/3) : u16ArrayLenth;

        for (uint16_t i=0; i<u16TransCnt; i++)
        {
            s32Index += sprintf((char*)(p_u8LogBuf+s32Index+3), "%02X ", *(p_u8Array+i));
        }
        s32Index += sprintf((char*)(p_u8LogBuf+s32Index+3), "\r\n");
        p_u8LogBuf[s32Index+3] = '\0';
        printf("%s",p_u8LogBuf);  
    } else {
        printf("%s %s %d error!\r\n",__FILE__, __func__, __LINE__);
    }
    
}

/******************************* END OF FILE **********************************/
