#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "LzmaDec.h"
#include "LzmaEnc.h"
#include "7zFile.h"

//#define LZMA_RAM_USE_DEBUG

#ifdef LZMA_RAM_USE_DEBUG
static int ram_used_size = 0;
static int ram_used_max = 0;
#endif

#ifndef _WIN32
#include "malloc.h"
uint32_t _msize(const void *mp)
{
    return(malloc_usable_size(mp));
}
#endif

static void *lzma_alloc(ISzAllocPtr p, size_t size)
{
    if(size == 0)
    {
        return NULL;
    }

#ifdef LZMA_RAM_USE_DEBUG
    ram_used_size += size;
    if (ram_used_max < ram_used_size)ram_used_max = ram_used_size;
    printf("ram used: now / max = %d / %d\n", ram_used_size, ram_used_max);
#endif

    return malloc(size);
}

static void lzma_free(ISzAllocPtr p, void *address)
{
    if(address != NULL)
    {
#ifdef LZMA_RAM_USE_DEBUG
        ram_used_size -= _msize(address);
        printf("ram used: now / max = %d / %d\n", ram_used_size, ram_used_max);
#endif

        free(address);
    }
}

ISzAlloc allocator = {lzma_alloc, lzma_free};

//dictSize:[12~30],设置字典大小
// 12,解码占用20352 = 16256 + 4096字节内存,2E12,已经是最低了
// 13,解码占用24448 = 16256 + 8192字节内存,2E13
// 16,解码占用81792 = 16256 + 65536字节内存,2E16
//level:压缩等级[0~9]
int lzma_compress(uint8_t *dest, int *destLen, const uint8_t *src, int srcLen, int dictSize, int level)
{
    CLzmaEncHandle enc;
    CLzmaEncProps props;
    SRes res;
    SizeT len;

    enc = LzmaEnc_Create(&allocator);
    if (enc == 0)
    {
        return SZ_ERROR_MEM;
    }

    LzmaEncProps_Init(&props);

    if (level > 9)level = 9;
    props.level = level;

    if (dictSize < 12)dictSize = 12;
    else if (dictSize > 30)dictSize = 30;
    props.dictSize = 1 << dictSize;

    props.writeEndMark = 1;
    res = LzmaEnc_SetProps(enc, &props);

    if (res == SZ_OK)
    {
        uint8_t header[LZMA_PROPS_SIZE + 8];
        size_t headerSize = LZMA_PROPS_SIZE;

        LzmaEnc_WriteProperties(enc, header, &headerSize);

        for (int i = 0; i < 8; i++)
        {
            header[headerSize++] = (uint8_t)(srcLen >> (8 * i));
        }

        memcpy(dest, header, headerSize);
        len = *destLen - headerSize;
        res = LzmaEnc_MemEncode(enc, dest + headerSize, &len, src, srcLen, 0, NULL, &allocator, &allocator);
        *destLen = len + headerSize;
    }

    LzmaEnc_Destroy(enc, &allocator, &allocator);
    return res;
}



