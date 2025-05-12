#ifndef __LZMA_COMPRESS_H__
#define __LZMA_COMPRESS_H__

#ifdef __cplusplus
extern "C"
{
#endif

int lzma_compress(uint8_t *dest, int *destLen, const uint8_t *src, int srcLen, int dictSize, int level);

#ifdef __cplusplus
}
#endif

#endif
