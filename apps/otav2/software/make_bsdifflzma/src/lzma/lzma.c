#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "LzmaDec.h"
#include "LzmaEnc.h"
#include "7zFile.h"

static void help(void)
{
    printf("Usage:\n");
    printf("lzma -c [file] [cmprs_file]          -compress \"file\" to \"cmprs_file\" \n");
    printf("lzma -d [cmprs_file] [dcmprs_file]   -dcompress \"cmprs_file\" to \"dcmprs_file\" \n\n");
    
    printf("lzmainfo [cmprs_file]                -show compress file infomation\n\n");
}

static int ram_used_size = 0;
static int ram_used_max = 0;

static void *lzma_alloc(ISzAllocPtr p, size_t size)
{
    if(size == 0)
    {
        return NULL;
    }

    ram_used_size += size;
    if (ram_used_max < ram_used_size)ram_used_max = ram_used_size;
    printf("ram used: now / max = %d / %d\n", ram_used_size, ram_used_max);

    return malloc(size);
}

static void lzma_free(ISzAllocPtr p, void *address)
{
    if(address != NULL)
    {
        ram_used_size -= _msize(address);
        printf("ram used: now / max = %d / %d\n", ram_used_size, ram_used_max);

        free(address);
    }
}

ISzAlloc allocator = {lzma_alloc, lzma_free};

//dictSize:[12~30],设置字典大小
// 12,解码占用20352 = 16256 + 4096字节内存,2E12,已经是最低了
// 13,解码占用24448 = 16256 + 8192字节内存,2E13
// 16,解码占用81792 = 16256 + 65536字节内存,2E16
//level:压缩等级[0~9]
static int lzma_compress(ISeqOutStream *outStream, ISeqInStream *inStream, UInt64 fileSize, int dictSize, int level)
{
    CLzmaEncHandle enc;
    CLzmaEncProps props;
    SRes res;

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
        Byte header[LZMA_PROPS_SIZE + 8];
        size_t headerSize = LZMA_PROPS_SIZE;

        LzmaEnc_WriteProperties(enc, header, &headerSize);

        for (int i = 0; i < 8; i++)
        {
            header[headerSize++] = (Byte)(fileSize >> (8 * i));
        }

        if (outStream->Write(outStream, header, headerSize) != headerSize)
        {
            res = SZ_ERROR_WRITE;
        }
        else
        {
            res = LzmaEnc_Encode(enc, outStream, inStream, NULL, &allocator, &allocator);
        }
    }

    LzmaEnc_Destroy(enc, &allocator, &allocator);
    return res;
}

static int lzma_decompress(ISeqOutStream *outStream, ISeqInStream *inStream)
{
    CLzmaDec state;
    UInt64 unpack_size;
    Byte header[LZMA_PROPS_SIZE + 8];
    size_t headerSize = sizeof(header);
    Byte inBuf[4096];
    Byte outBuf[4096];
    size_t inPos = 0, inSize = 0, outPos = 0;
    SRes res;

    inStream->Read(inStream, header, &headerSize);

    for(int i = 0; i < 8; i++)
    {
        unpack_size += (UInt64)(header[LZMA_PROPS_SIZE + i] << (i * 8));
    }

    LzmaDec_Construct(&state);
    LzmaDec_Allocate(&state, header, LZMA_PROPS_SIZE, &allocator);
    LzmaDec_Init(&state);

    while(1)
    {
        if(inPos == inSize)
        {
            inSize = 4096;
            inPos = 0;
            inStream->Read(inStream, inBuf, &inSize);
        }
        {
            SizeT inProcessed = inSize - inPos;
            SizeT outProcessed = 4096 - outPos;
            ELzmaFinishMode finishMode = LZMA_FINISH_ANY;
            ELzmaStatus status;

            if (outProcessed > unpack_size)
            {
                outProcessed = (SizeT)unpack_size;
                finishMode = LZMA_FINISH_END;
            }

            res = LzmaDec_DecodeToBuf(&state, outBuf + outPos, &outProcessed,
                                      inBuf + inPos, &inProcessed, finishMode, &status);
            inPos += inProcessed;
            outPos += outProcessed;
            unpack_size -= outProcessed;

            if (outStream)
            {
                if (outStream->Write(outStream, outBuf, outPos) != outPos)
                {
                    return SZ_ERROR_WRITE;
                }
            }
            outPos = 0;

            if (res != SZ_OK)
            {
                return res;
            }

            if (inProcessed == 0 && outProcessed == 0)
            {
                if(status != LZMA_STATUS_FINISHED_WITH_MARK)
                {
                    return SZ_ERROR_DATA;
                }
                return res;
            }
        }
    }

    LzmaDec_Free(&state, &allocator);
    return res;
}

int lzma_file_info(const char* file_in)
{
    CLzmaProps props;
    UInt32 dic_size;
    Byte props_buff[LZMA_PROPS_SIZE + sizeof(UInt64)];
    SizeT dcmprs_size = 0;
    Byte d;
    int fd_in = -1;
    int ret = 0;

    fd_in = open(file_in, O_RDONLY, 0);
    if (fd_in < 0)
    {
        printf("open the lzma file: %s erroe\n", file_in);
        ret = -1;
        goto _exit;
    }

    read(fd_in, props_buff, LZMA_PROPS_SIZE + sizeof(UInt64));

    for (int i = 0; i < 8; i++)
        dcmprs_size += (UInt64)props_buff[LZMA_PROPS_SIZE + i] << (i * 8);

    dic_size = props_buff[1] | ((UInt32)props_buff[2] << 8) |
        ((UInt32)props_buff[3] << 16) | ((UInt32)props_buff[4] << 24);

    if (dic_size < (1 << 12))
    {
        dic_size = (1 << 12);
    }

    props.dicSize = dic_size;

    d = props_buff[0];

    if (d >= (9 * 5 * 5))
    {
        printf("lzmainfo: %s: Not a lzma file\n", file_in);
        goto _exit;
    }

    props.lc = (Byte)(d % 9);
    d /= 9;
    props.pb = (Byte)(d / 5);
    props.lp = (Byte)(d % 5);

    printf("\n%s\n", file_in);
    printf("Uncompressed size:              %d MB (%d bytes)\n", (UInt32)(dcmprs_size / 1024 / 1024), dcmprs_size);
    printf("Dictionary size:                %d MB (%d bytes)\n", (UInt32)(props.dicSize / 1024 / 1024), props.dicSize);
    printf("Literal context bits (lc):      %d\n", props.lc);
    printf("Literal pos bits (lp):          %d\n", props.lp);
    printf("Number of pos bits (pb):        %d\n\n", props.pb);

_exit:
    if (fd_in >= 0)
    {
        close(fd_in);
    }
    return ret;
}

int lzma_file_compress(const char* file_in, const char* file_out, int dictSize, int level)
{
    CFileSeqInStream inStream;
    CFileOutStream outStream;
    int ret = 0;
    UInt64 in_fileSize;

    FileSeqInStream_CreateVTable(&inStream);
    File_Construct(&inStream.file);

    FileOutStream_CreateVTable(&outStream);
    File_Construct(&outStream.file);

    if (InFile_Open(&inStream.file, file_in) != 0)
    {
        printf("open the input file: %s error\n", file_in);
        ret = -1;
        goto _err1;
    }

    if (OutFile_Open(&outStream.file, file_out) != 0)
    {
        printf("open the output file: %s error\n", file_out);
        ret = -1;
        goto _err2;
    }

    File_GetLength(&inStream.file, &in_fileSize);
    if (lzma_compress(&outStream.vt, &inStream.vt, in_fileSize, dictSize, level) != SZ_OK)
    {
        printf("lzma compress file error!\n");
        goto _err3;
    }
    printf("lzma compress file success!\n");

_err3:
    File_Close(&outStream.file);

_err2:
    File_Close(&inStream.file);

_err1:
    return ret;
}

int lzma_file_decompress(const char* file_in, const char* file_out)
{
    CFileSeqInStream inStream;
    CFileOutStream outStream;
    int ret = 0;

    FileSeqInStream_CreateVTable(&inStream);
    File_Construct(&inStream.file);

    FileOutStream_CreateVTable(&outStream);
    File_Construct(&outStream.file);

    if (InFile_Open(&inStream.file, file_in) != 0)
    {
        printf("open the input file: %s erroe\n", file_in);
        ret = -1;
        goto _err1;
    }

    if (OutFile_Open(&outStream.file, file_out) != 0)
    {
        printf("open the output file: %s erroe\n", file_out);
        ret = -1;
        goto _err2;
    }

    if (lzma_decompress(&outStream.vt, &inStream.vt) != SZ_OK)
    {
        printf("lzma decompress file error!\n");
        goto _err3;
    }
    printf("lzma decompress file success!\n");

_err3:
    File_Close(&outStream.file);

_err2:
    File_Close(&inStream.file);

_err1:
    return ret;
}



