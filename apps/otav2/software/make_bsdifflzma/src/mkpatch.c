#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "bsdiff.h"
#include "lzma_compress.h"

#ifndef _WIN32
#define O_BINARY 0
#endif

uint8_t* file_read(const char *path, uint32_t *len);

static int diff_data_write(struct bsdiff_stream* stream, const void* buffer, int size)
{
    FILE *pf;
    uint8_t *dp = (uint8_t*)buffer;

    pf = (FILE*) stream->opaque;
    fwrite((void*) buffer, size, 1, pf);

    return 0;
}

uint8_t *mkpatch(const char *old_file, const char *new_file, uint32_t *patch_size)
{
    int fd;
    uint8_t *old = NULL, *new = NULL, *diff = NULL, *diff_lz = NULL;
    uint8_t *patch = NULL;
    uint32_t oldsize, newsize, diffsize, patchsize = 0;
    int lzsize;
    uint8_t buf[8];
    FILE *pf = NULL;
    struct bsdiff_stream stream;

    stream.malloc = malloc;
    stream.free = free;
    stream.write = diff_data_write;

    /* Allocate oldsize+1 bytes instead of oldsize bytes to ensure
            that we never try to malloc(0) and get a NULL pointer */
    old = file_read(old_file, &oldsize);
    if(old == NULL)goto _exit;

    /* Allocate newsize+1 bytes instead of newsize bytes to ensure
            that we never try to malloc(0) and get a NULL pointer */
    new = file_read(new_file, &newsize);
    if(new == NULL)goto _exit;

    /* Create the patch file */
    if ((pf = fopen("diff_raw.bin", "wb")) == NULL)
    {
        printf("%s\n", "diff_raw.bin");
        goto _exit;
    }
    stream.opaque = pf;
    if (bsdiff(old, oldsize, new, newsize, &stream))
    {
        printf("bsdiff\n");
    }
    fclose(pf);
    
    //读出数据并压缩
    if (((fd = open("diff_raw.bin", O_RDONLY | O_BINARY, 0)) < 0) ||
            ((diffsize = lseek(fd, 0, SEEK_END)) == -1) ||
            ((diff = malloc(diffsize + 1)) == NULL) ||
            (lseek(fd, 0, SEEK_SET) != 0) ||
            (read(fd, diff, diffsize) != diffsize) ||
            (close(fd) == -1)) 
    {
        printf("%s\n", "diff_raw.bin");
        goto _exit;
    }
    lzsize = diffsize + 10 * 1024;
    diff_lz = malloc(lzsize);
    lzma_compress(diff_lz, &lzsize, diff, diffsize, 12, 9);
	
	//删除中间文件
	remove("diff_raw.bin");
    
    //写入文件  
    patch = malloc(16 + sizeof (buf) + lzsize + 1);
    patchsize = 0;
    if(patch != NULL)
    {
        memcpy(&patch[patchsize], "ENDSLEY/BSDIFF43", 16);
        patchsize += 16;
        
        offtout(newsize, buf);        
        memcpy(&patch[patchsize], buf, sizeof (buf));
        patchsize += sizeof (buf);
        
        memcpy(&patch[patchsize], diff_lz, lzsize);
        patchsize += lzsize;
        *patch_size = patchsize;
    }
     
    /* Free the memory we used */
_exit:
    if(old != NULL)free(old);
    if(new != NULL)free(new);
    if(diff != NULL)free(diff);
    if(diff_lz != NULL)free(diff_lz);

    return patch;
}


uint8_t *mklzma(const char *src_file, uint32_t *out_size)
{
    int fd;
    uint32_t src_size;
    uint8_t *src_data = NULL, *lzma_data = NULL;
    int lzsize;
    
    //读出数据并压缩
    if (((fd = open(src_file, O_RDONLY | O_BINARY, 0)) < 0) ||
            ((src_size = lseek(fd, 0, SEEK_END)) == -1) ||
            ((src_data = malloc(src_size + 1)) == NULL) ||
            (lseek(fd, 0, SEEK_SET) != 0) ||
            (read(fd, src_data, src_size) != src_size) ||
            (close(fd) == -1)) 
    {
        printf("%s\n", src_file);
        goto _exit;
    }
    lzsize = src_size + 10 * 1024;
    lzma_data = malloc(lzsize);
    lzma_compress(lzma_data, &lzsize, src_data, src_size, 12, 9);

    *out_size = (uint32_t)lzsize;
    
    /* Free the memory we used */
_exit:
    if(src_data != NULL)free(src_data);

    return lzma_data;
}

