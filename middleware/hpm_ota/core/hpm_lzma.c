#include "lzma_decompress.h"
#include "vFile.h"
#include "hpm_flash.h"
#include "hpm_diffconfig.h"
#include "bspatch.h"
#include "hpm_log.h"
#include "hpm_diffmgr.h"

#define DCOMPRESS_BUFFER_SIZE   1024

int hpm_lzma_upgrade_restore(const uint32_t lzmaimgaddr, uint32_t lzmaimglen, uint32_t newimgaddr, uint32_t newimglen)
{
    vFile *fp;
    uint8_t read_buffer[DCOMPRESS_BUFFER_SIZE];
    uint32_t offset_len = 0, read_len;
    (void)newimglen;

    fp = vfopen((const uint8_t *)lzmaimgaddr, lzmaimglen);

    if (fp == NULL)
    {
        return (-1);
    }

    hpm_flash_empty_erase_mask();

    while(1)
    {
        read_len = lzma_decompress_read(fp, read_buffer, DCOMPRESS_BUFFER_SIZE);
        if(read_len > 0)
        {
            hpm_flash_erase_write_of_mask(newimgaddr + offset_len, read_buffer, read_len);
            offset_len += read_len;
        }
        else
        {
            break;
        }
    }
    //释放内存
    lzma_decompress_finish();
    vfclose(fp);

    return 0;
}