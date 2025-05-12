#include "cmdline.h"
#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include <time.h>
#include "lzma_compress.h"
#include "mkpatch.h"

using namespace std;

uint32_t file_get_length(FILE *fp)
{
    long cur_pos;
    uint32_t len = 0;

    //取得当前文件流的读取位置
    cur_pos = ftell(fp);
    //将文件流的读取位置设为文件末尾
    fseek(fp, 0, SEEK_END);
    //获取文件末尾的读取位置,即文件大小
    len = ftell(fp);
    //将文件流的读取位置还原为原先的值
    fseek(fp, cur_pos, SEEK_SET);

    return len;
}

extern "C" {
uint8_t* file_read(const char *path, uint32_t *len)
{
    FILE *fp;
    unsigned char *dp = NULL;
    unsigned char *rp = NULL;
    unsigned int file_len;

    fp = fopen(path, "rb");
    if (fp != NULL)
    {
        file_len = file_get_length(fp);
        dp = (unsigned char*) malloc(file_len + 1);
        if (dp != NULL)
        {
            fread(dp, 1, file_len, fp);
        }
        else printf("malloc error!\n");
    }
    else
    {
        goto __exit;
    }
    fclose(fp);
    
    rp = dp;
    *len = file_len;

__exit:
    return (rp);
}
}

void file_write(const char *path, void *buffer, uint32_t len)
{
    FILE *fp;
    fp = fopen(path, "wb");
    if (fp != NULL)
    {
        fwrite(buffer, len, 1, fp);
    }
    fclose(fp);
}

int main(int argc, char *argv[])
{
    cmdline::parser a;

    a.add<string>("type", 't', "type", true, "");
    a.add<string>("old", 'o', "input old file name", true, "");
    a.add<string>("new", 'n', "input new file name", true, "");
    a.add<string>("patch", 'p', "output patch file name", true, "");

    a.parse_check(argc, argv);

    //0: diff, 1: lzma
    if(memcmp(a.get<string>("type").c_str(), "0", 1) == 0)
    {
        uint32_t addr, ep, patch_size;
        uint8_t *patch;
        
        patch = mkpatch(a.get<string>("old").c_str(), a.get<string>("new").c_str(), &patch_size);
        if(patch != NULL)
        {
            file_write(a.get<string>("patch").c_str(), patch, patch_size);
            cout << "success!" << endl;
            free(patch);
        }
    }
    else if(memcmp(a.get<string>("type").c_str(), "1", 1) == 0)
    {
        uint32_t addr, ep, patch_size;
        uint8_t *patch;
        
        patch = mklzma(a.get<string>("old").c_str(), &patch_size);
        if(patch != NULL)
        {
            file_write(a.get<string>("patch").c_str(), patch, patch_size);
            cout << "success!" << endl;
            free(patch);
        }
    }

    return 0;
}

