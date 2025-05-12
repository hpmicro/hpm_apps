//#include "bs_type.h"
#include "lzma_decompress.h"
#include "vFile.h"
#include "hpm_flash.h"
#include "hpm_diffconfig.h"
#include "bspatch.h"
#include "hpm_log.h"
#include "hpm_diffmgr.h"

#define DCOMPRESS_BUFFER_SIZE   4096
static uint8_t *diff_data_buff;
static int diff_data_len, diff_data_fp;
static int new_data_fp;

static int patch_data_read(const struct bspatch_stream *stream, void *buffer, int length)
{
    uint8_t *dp = (uint8_t *)buffer;
    vFile *pf;

    pf = (vFile *) stream->opaque_r;

    for (int i = 0; i < length; i++)
    {
        if (diff_data_len == 0)
        {
            diff_data_len = lzma_decompress_read(pf, diff_data_buff, DCOMPRESS_BUFFER_SIZE);
            if (diff_data_len > 0)
            {
                diff_data_fp = 0;
            }
            else
            {
                HPM_LOG_ERR("err%d", __LINE__);
                return -1;
            }
        }
        if (diff_data_len > 0)
        {
            *(dp++) = diff_data_buff[diff_data_fp++];
            diff_data_len--;
        }
    }

    return 0;
}

static int new_data_write(const struct bspatch_stream *stream, void *buffer, int length)
{
    uint32_t file_addr;

    file_addr = *((uint32_t *)stream->opaque_w);

    if(hpm_flash_erase_write_of_mask(file_addr + new_data_fp, buffer, length) < 0)
    {
        HPM_LOG_ERR("flash write fail!\r\n");
        return -1;
    }
    new_data_fp += length;

    return 0;
}

static void patch_data_read_finish(void)
{
    lzma_decompress_finish();

}

/**
 * @brief 用户使用差分升级时唯一需要关心的接口
 * 
 * @param oldimgaddr 设备中执行区代码所在的地址，用户可指定flash执行区的地址，方便算法读出来当前
 *            运行中的代码
 * @param oldimglen 设备中执行区代码的长度，用户可在差分包bin头获取
 * @param diffimgaddr 设备中已经下载的差分包所在的flash地址，或者ram地址，只要能让算法读出来即可
 *              注意，下载的差分包自带image_header_t格式的文件头，真正的差分包需要偏
 *              移sizeof(image_header_t)的长度
 * @param diffimglen 设备中已经下载的差分包的长度，用户可在差分包bin头获取
 * @param newimgaddr 还原的完整bin下载的位置
 * @return int <0 失败，>= 0 成功
 */
int hpm_diff_upgrade_restore(const uint32_t oldimgaddr, uint32_t oldimglen, const uint32_t diffimgaddr, uint32_t diffimglen, uint32_t newimgaddr)
{
    vFile *patch_fp;
    struct bspatch_stream stream;
    uint8_t header[24];
    int64_t newsize;

    //初始化全局变量
    diff_data_len = 0;
    diff_data_fp = 0;
    new_data_fp = 0;
    patch_fp = vfopen((const uint8_t *)diffimgaddr, diffimglen);

    HPM_LOG_INFO("oldimg addr:0x%08x, oldimg len:%d, diff img addr:0x%08x, diff img len:%d, new img addr:0x%08x\r\n",
                 oldimgaddr, oldimglen, diffimgaddr, diffimglen, newimgaddr);

    if (patch_fp == NULL)
    {
        return (-1);
    }

    //读取差分文件头
    vfread(patch_fp, header, sizeof(header));
    if (memcmp(header, "ENDSLEY/BSDIFF43", 16) != 0)
    {
        HPM_LOG_ERR("ENDSLEY/BSDIFF43 err: %s", header);
        for (uint32_t i = 0; i < sizeof(header); i++)
        {
            HPM_LOG_RAW("%02X ", header[i]);
        }
        return (-2);
    }

    //计算新固件长度
    newsize = offtin(header + 16);
    if (newsize < 0)
    {
        HPM_LOG_ERR("newsize err");
        return (-3);
    }
    HPM_LOG_INFO("newimg len:%lld\r\n", newsize);

    //分配内存
    diff_data_buff = ota_malloc(DCOMPRESS_BUFFER_SIZE);
    if (diff_data_buff == NULL)
    {
        HPM_LOG_ERR("\r\nmalloc err");
        return (-4);
    }

    hpm_flash_empty_erase_mask();

    //准备合并文件
    stream.read = patch_data_read;
    stream.opaque_r = (void *)patch_fp;
    stream.write = new_data_write;
    stream.opaque_w = &newimgaddr;

    int res = bspatch((const uint8_t *)oldimgaddr, oldimglen, newsize, &stream);

    //释放内存
    patch_data_read_finish();
    ota_free(diff_data_buff);
    vfclose(patch_fp);

    return res == 0 ? ((int)newsize) : -1;
}