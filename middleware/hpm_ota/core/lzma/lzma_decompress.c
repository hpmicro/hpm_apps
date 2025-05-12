#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "LzmaDec.h"
#include "LzmaEnc.h"
#include "7zFile.h"
#include "vFile.h"
#include "hpm_log.h"
#include "ota_kconfig.h"

//#define LZMA_RAM_USE_DEBUG

#ifdef LZMA_RAM_USE_DEBUG
static int ram_used_size = 0;
static int ram_used_max = 0;
#endif

void *lzma_alloc(ISzAllocPtr p, size_t size)
{
    (void) p;
    void *mp;

    if (size == 0)
    {
        return NULL;
    }

    mp = ota_malloc(size);

#ifdef LZMA_RAM_USE_DEBUG
    ram_used_size += _msize(mp);
    if (ram_used_max < ram_used_size)ram_used_max = ram_used_size;
    printk("ram used: now / max = %d / %d\n", ram_used_size, ram_used_max);
#endif
    
    return mp;
}

void lzma_free(ISzAllocPtr p, void *address)
{
    (void) p;
    if (address != NULL)
    {
#ifdef LZMA_RAM_USE_DEBUG
        ram_used_size -= _msize(address);
        printk("ram used: now / max = %d / %d\n", ram_used_size, ram_used_max);
#endif
        ota_free(address);
    }
}

ISzAlloc allocator = {lzma_alloc, lzma_free};
static CLzmaDec *lz_state = NULL; 

static int lzma_decompress_init(vFile* pf)
{
    if (lz_state != NULL) return 0;

    extern ISzAlloc allocator;
    UInt64 unpack_size;
    uint8_t header[LZMA_PROPS_SIZE + 8];;
    size_t headerSize = sizeof(header);

    vfread(pf, header, headerSize);
    for(int i = 0; i < 8; i++)
    {
        unpack_size += (UInt64)(header[LZMA_PROPS_SIZE + i] << (i * 8));
    }
    
    //分配解码内存
    lz_state = (CLzmaDec *)lzma_alloc(NULL, sizeof(CLzmaDec));

    if (lz_state != NULL)
    {
        LzmaDec_Construct(lz_state);
        LzmaDec_Allocate(lz_state, header, LZMA_PROPS_SIZE, &allocator);
        LzmaDec_Init(lz_state);
        return 0;
    }

    return -1;
}

int lzma_decompress_read(vFile* pf, uint8_t *buffer, int size)
{
    /* Start to decompress file  */
    uint32_t position, file_size;
    size_t dcmprs_size = 0;
    uint8_t *inBuf;
    SizeT inProcessed;
    SizeT outProcessed = size;
    ELzmaFinishMode finishMode = LZMA_FINISH_ANY;
    ELzmaStatus status;
    SRes res;

    res = lzma_decompress_init(pf);
    if (res != 0)
    {
        HPM_LOG_ERR("err%d", __LINE__);
		return -1;
    }

    //获取当前文件读指针
    inBuf = vfgetpos(pf, &position);
    //检查文件还剩下多少字节
    file_size = vfgetlen(pf);
    if ((position + size) > file_size)
    {
        inProcessed = file_size - position;
    }
    else
    {
        inProcessed = size;
    }
    HPM_LOG_INFO("P:%d%%\r\n", position*100/file_size);

    //解压数据
    if(inProcessed >= 0)
    {
        res = LzmaDec_DecodeToBuf(lz_state, buffer, &outProcessed, inBuf, &inProcessed, finishMode, &status);
        dcmprs_size = outProcessed;

        //重新设置文件读指针
        position += inProcessed;
        vfsetpos(pf, position);
    }
    return dcmprs_size;
}

void lzma_decompress_finish(void)
{
    LzmaDec_Free(lz_state, &allocator);
    
    if(lz_state != NULL)
    {
        lzma_free(NULL, lz_state);
        lz_state = NULL;
    }
}
