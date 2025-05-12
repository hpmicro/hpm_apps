#ifndef __LZMA_SAMPLE_H__
#define __LZMA_SAMPLE_H__

#ifdef __cplusplus
extern "C"
{
#endif

int lzma_file_info(const char* file_in);
int lzma_file_compress(const char* file_in, const char* file_out, int dictSize, int level);
int lzma_file_decompress(const char* file_in, const char* file_out);

#ifdef __cplusplus
}
#endif

#endif
