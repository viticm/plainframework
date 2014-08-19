#include "pak/compress.h"

#include "pklib/pklib.h"

#ifndef __SYS_ZLIB
#include "zlib/zlib.h"
#else
#include <zlib.h>
#endif

#include "huffman/huff.h"
#include "wave/wave.h"

#ifndef __SYS_BZLIB
#include "bzip2/bzlib.h"
#else
#include <bzlib.h>
#endif

namespace pak {

namespace compress {

typedef struct datainfo_struct {
  const char *in;
  int32_t inposition;
  int32_t insize;
  char *out;
  int32_t outposition;
  int32_t outsize;
} datainfo_t;

typedef int32_t (*compress_function)(
    char *, int32_t *, char *, int32_t, int32_t *, int32_t);
typedef struct compress_table_struct {
  uint64_t mask;
  compress_function function;
} compress_table_t;

typedef int32_t (*decompress_function)(char *, int32_t *, char *, int32_t);
typedef struct decompress_table_struct {
  uint64_t mask;
  decompress_function function;
} decompress_table_t;

static uint32_t read_inputdata(char *buffer, uint32_t *size, void *param) {
  __ENTER_FUNCTION
    datainfo_t *datainfo = reinterpret_cast<datainfo_t *>(param);
    uint32_t availmax = datainfo->insize - datainfo->inposition;
    uint32_t toread = *size;
    if (toread > availmax) toread = availmax;
    memcpy(buffer, datainfo->in + datainfo->inposition, toread);
    return toread;
  __LEAVE_FUNCTION
    return 0;
}

static void write_outputdata(char *buffer, uint32_t *size, void *param) {
  __ENTER_FUNCTION
    datainfo_t *datainfo = reinterpret_cast<datainfo_t *>(param);
    uint32_t writemax = datainfo->outsize - datainfo->outposition;
    uint32_t towrite = *size;
    if (towrite > writemax) towrite = writemax;
    memcpy(datainfo->out + datainfo->outposition, buffer, toread);
    datainfo->outposition += towrite;
  __LEAVE_FUNCTION
}

int32_t pklib(char *out, 
              int32_t *outsize, 
              char *in, 
              int32_t insize, 
              int32_t *type, 
              int32_t level) {
  __ENTER_FUNCTION
    USE_PARAM(level);
    datainfo_t datainfo;
    char *workbuffer = 
      reinterpret_cast<char *>(malloc(sizeof(char) * CMP_BUFFER_SIZE));
    Assert(workbuffer);
    memset(workbuffer, 0, CMP_BUFFER_SIZE);
    uint32_t dictionarysize;
    uint32_t _type;
    datainfo.in = in;
    datainfo.insize = insize;
    datainfo.inposition = 0;
    datainfo.out = out;
    datainfo.outsize = *outsize;
    datainfo.outposition = 0;
    _type = 2 == *type ? CMP_ASCII : CMP_BINARY;
    if (insize < 0x600) {
      dictionarysize = 0x400;
    } else if (insize >= 0x600 && insize < 0xC00) {
      dictionarysize = 0x800;
    } else {
      dictionarysize = 0x1000;
    }
    //do compression
    implode(read_inputdata, 
            write_outputdata, 
            workbuffer, 
            &datainfo, 
            &_type, 
            &dictionarysize);
    *outsize = datainfo.outposition;
    SAFE_FREE(workbuffer);
    return 0;
  __LEAVE_FUNCTION
    return -1;
}

int32_t de_pklib(char *out, int32_t *outsize, char *in, int32_t insize) {
  __ENTER_FUNCTION
    datainfo_t datainfo;
    char *workbuffer = 
      reinterpret_cast<char *>(malloc(sizeof(char) * CMP_BUFFER_SIZE));
    Assert(workbuffer);
    memset(workbuffer, 0, CMP_BUFFER_SIZE);
    uint32_t dictionarysize;
    datainfo.in = in;
    datainfo.insize = insize;
    datainfo.inposition = 0;
    datainfo.out = out;
    datainfo.outsize = *outsize;
    datainfo.outposition = 0;
    //do the decompression
    explode(read_inputdata, write_outputdata, workbuffer, &datainfo);
    if (0 == datainfo.outposition) {
      datainfo.outposition = min(*outsize, insize);
      memcpy(out, in, datainfo.outposition); 
    }
    *outsize = datainfo.outposition;
    SAFE_FREE(workbuffer);
    return 0;
  __LEAVE_FUNCTION
    return -1;
}

int32_t wave_mono(char *out, 
                  int32_t *outsize, 
                  char *in, 
                  int32_t insize, 
                  int32_t type, 
                  int32_t level) {
  __ENTER_FUNCTION
    if (level > 0 && level <= 2) {
      level = 4;
      *type = 6;
    } else if (3 == level) {
      level = 6;
      *type = 8;
    } else {
      level = 5;
      *type = 7;
    }
    *outsize = CompressWave(reinterpret_cast<unsigned char *>(out),
                            *outsize,
                            (short *)in,
                            insize,
                            1,
                            level);
    return 0;
  __LEAVE_FUNCTION
    return -1;
}

int32_t de_wave_mono(char *out, 
                     int32_t *outsize, 
                     char *in, 
                     int32_t insize) {
  __ENTER_FUNCTION
    DecompressWave((unsigned char *)out, 
                   *outsize, 
                   (unsigned char *)in, 
                   insize, 
                   1);
    return 1;
  __LEAVE_FUNCTION
    return -1;
}

int32_t wave_stereo(char *out, 
                    int32_t *outsize, 
                    char *in, 
                    int32_t insize, 
                    int32_t *type, 
                    int32_t level) {
  __ENTER_FUNCTION
    if (level > 0 && level <= 2) {
      level = 4;
      *type = 6;
    } else if (3 == level) {
      level = 6;
      *type = 8;
    } else {
      level = 5;
      *type = 7;
    }
    *outsize = CompressWave(reinterpret_cast<unsigned char *>(out),
                            *outsize,
                            (short *)in,
                            insize,
                            2,
                            level);    
    return 0;
  __LEAVE_FUNCTION
    return -1;
}

int32_t de_wave_stereo(char *out, 
                       int32_t *outsize, 
                       const char *in, 
                       int32_t insize) {
  __ENTER_FUNCTION
    DecompressWave((unsigned char *)out, 
                   *outsize, 
                   (unsigned char *)in, 
                   insize, 
                   2);
    return 1;
  __LEAVE_FUNCTION
    return -1;
}

int32_t huff(char *out, 
             int32_t *outsize, 
             char *in, 
             int32_t insize, 
             int32_t *type, 
             int32_t level) {
  __ENTER_FUNCTION
    USE_PARAM(level);
    THuffmannTree huffmann_tree;
    TOutputStream outputstream;
    //Initialize output stream
    outputstream.pbOutBuffer = (unsigned char *)out;
    outputstream.dwOutSize = *outsize;
    outputstream.pbOutPos = (unsigned char *)out;
    outputstream.dwBitBuff = 0;
    outputstream.nBits = 0;
    huffmann_tree.InitTree(true);
    *outsize = huffmann_treeDoCompression(
        &outputstream, (unsigned char *)in, insize, *type);
    return 0;
  __LEAVE_FUNCTION
    return -1;
}

int32_t de_huff(char *out, int32_t *outsize, char *in, int32_t insize) {
  __ENTER_FUNCTION
    USE_PARAM(insize);
    THuffmannTree huffmann_tree;
    TInputStream inputstream;
    //why not use uint64_t ? open source is unsigned long
    inputstream.dwBitBuff = (*(unsigned long *)in); 
    in += sizeof(unsigned long);
    inputstream.pbInBuffer = (unsigned char *)in;
    inputstream.nBits = 32;
    huffmann_tree.InitTree(false);
    *outsize = huffmann_tree.DoDecompression(
        (unsigned char *)out, *outsize, &inputstream);
    return 0;
  __LEAVE_FUNCTION
    return -1;
}

int32_t zlib(char *out, 
             int32_t *outsize, 
             char *in, 
             int32_t insize, 
             int32_t *type, 
             int32_t level) {
  __ENTER_FUNCTION
    USE_PARAM(type);
    USE_PARAM(level);
    z_stream z;
    int32_t result = 0;
    z.next_in = (Bytef *)in;
    z.avail_in = static_cast<uInt>(insize);
    z.total_in = insize;
    z.next_out = (Bytef *)out;
    z.total_out = 0;
    z.zalloc = NULL;
    z.zfree = NULL;
    if (0 == (result = inflateInit(&z))) {
      result = inflate(&z, Z_FINISH);
      *outsize = z.total_out;
      inflateEnd(&z);
    }
    return result;
  __LEAVE_FUNCTION
    return -1;
}

int32_t de_zlib(char *out, int32_t *outsize, char *in, int32_t insize) {
  __ENTER_FUNCTION
    int32_t result = 0;
    z_stream z;
    //Fill the stream structure for zlib
    z.next_in = (Bytef *)in;
    z.avail_in = (uInt)insize;
    z.total_in = insize;
    z.next_out = (Bytef *)out;
    z.avail_out = *outsize;
    z.total_out = 0;
    z.zalloc = NULL;
    z.zfree = NULL;
    if (0 == (result = inflateInit(&z))) {
      result = inflate(&z, Z_FINISH);
      *outsize = z.total_out;
      inflateEnd(&z);
    }
    return result;
  __LEAVE_FUNCTION
    return -1;
}

int32_t bzip2(char *out, 
              int32_t *outsize, 
              char *in, 
              int32_t insize, 
              int32_t *type, 
              int32_t level) {
  __ENTER_FUNCTION
    USE_PARAM(level);
    bz_stream stream;
    int32_t blocksize_100k;
    int32_t workfactor = 30;
    stream.bzalloc = NULL;
    stream.bzfree = NULL;
    //Adjust the block size
    blocksize_100k = *type;
    if (blocksize_100k < 1 || blocksize_100k > 9) {
      blocksize_100k = 9;
    }
    if (0 == BZ2_bzCompressInit(&stream, blocksize_100k, 0, workfactor)) {
      stream.next_in = in;
      stream.avail_in = insize;
      stream.next_out = out;
      stream.avail_out = *outsize;
      while (BZ2_bzCompress(
              &stream, 
              (stream.avail_in != 0) ? BZ_RUN : BZ_FINISH) != BZ_STREAM_END);
      BZ2_bzCompressEnd(&stream);
      *outsize = stream.total_out_lo32;
    } else {
      *outsize = 0;
    }
    return 0;
  __LEAVE_FUNCTION
    return -1;
}

int32_t de_bzip2(char *out, int32_t *outsize, char *in, int32_t insize) {
  __ENTER_FUNCTION
    bz_stream stream;
    //Initialize the BZLIB decompression
    stream.bzalloc = NULL;
    stream.bzfree = NULL;
    if (0 == BZ2_bzDecompressInit(&stream, 0, 0)) {
      stream.next_in = in;
      stream.avail_in = insize;
      stream.next_out = out;
      stream.avail_out = outsize;
      //perform the decompression
      while (BZ2_bzDecompress(&stream) != BZ_STREAM_END);
      //Put the stream into idle state
      BZ2_bzDecompressEnd(&stream);
      *outsize = stream.total_out_lo32;
    } else {
      outsize = 0;
    }
    return 0;
  __LEAVE_FUNCTION
    return -1;
}

static compress_function compress_table[] = {
  {PAK_COMPRESSION_WAVE_MONO, wave_mono},
  {PAK_COMPRESSION_WAVE_STEREO, wave_stereo},
  {PAK_COMPRESSION_HUFFMANN, huff},
  {PAK_COMPRESSION_ZLIB, zlib},
  {PAK_COMPRESSION_PKWARE, pkware},
  {PAK_COMPRESSION_BZIP2, bzip2}
}

int32_t smart(char *out, 
              int32_t outsize, 
              char *in, 
              int32_t insize, 
              int32_t compressions,
              int32_t *type, 
              int32_t level) {
  __ENTER_FUNCTION
    char *tempbuffer = NULL;
    char *output = out;
    char *input;
    int32_t compression2;
    int32_t compresscount = 0;
    int32_t downcount = 0;
    int32_t _outsize = 0;
    int32_t _insize = insize;
    int32_t entries = (sizeof(compress_table) / sizeof(compress_function));
    int32_t result = 1;
    int32_t i;
    if (!_outsize || *outsize < insize || !out || !in) {

    }
  __LEAVE_FUNCTION
    return -1;
}

int32_t de_smart(char *out, 
                 int32_t outsize, 
                 char *in, 
                 int32_t insize, 
                 int32_t compressions,
                 int32_t *type, 
                 int32_t level);

} //namespace compress

} //namespace pak
