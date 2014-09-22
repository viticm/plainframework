#include "pak/file.h"
#include "pak/util.h"

int32_t g_pakerror;

namespace pak {

namespace util {

void set_lasterror(int32_t error) {
  g_pakerror = error;
}

int32_t get_lasterror() {
  return g_pakerror;
}

const char *get_last_errorstr(int32_t error) {
  const char *errorstr;
  switch (error) {
    case PAK_ERROR_INVALID_FUNCTION:
      errorstr = "function not implemented";
      break;
    case PAK_ERROR_FILE_NOT_FOUND:
      errorstr = "file not found";
      break;
    case PAK_ERROR_ACCESS_DENIED:
      errorstr = "access denied";
      break;
    case PAK_ERROR_NOT_ENOUGH_MEMORY:
      errorstr = "not enough memory";
      break;
    case PAK_ERROR_BAD_FORMAT:
      errorstr = "bad format";
      break;
    case PAK_ERROR_NO_MORE_FILES:
      errorstr = "no more files";
      break;
    case PAK_ERROR_HANDLE_EOF:
      errorstr = "access beyound EOF";
      break;
    case PAK_ERROR_HANDLE_DISK_FULL:
      errorstr = "no space left on device";
      break;
    case PAK_ERROR_INVALID_PARAMETER:
      errorstr = "invalid parameter";
      break;
    case PAK_ERROR_DISK_FULL:
      errorstr = "no space left on device";
      break;
    case PAK_ERROR_ALREADY_EXISTS:
      errorstr = "file exists";
      break;
    case PAK_ERROR_CAN_NOT_COMPLETE:
      errorstr = "operation cannot be completed";
      break;
    default:
      errorstr = "unkown error";
      break;
  }
  return errorstr;
}

void freearchive(archive_t *&archive) {
  __ENTER_FUNCTION
    if (archive) {
      SAFE_FREE(archive->blocksize.buffer);
      SAFE_FREE(archive->block_table);
      SAFE_FREE(archive->hashkey_table);
      if (archive->fp != HANDLE_INVALID) {
        file::closeex(archive->fp);
      }
      freeattributes(archive->attribute);
      for (int32_t i = 0; i < archive->header.hashtable_size; ++i) {
        if (archive->listfiles) {
          filenode_t *node = archive->listfiles[i];
          if ((int64_t *)node < (int64_t *)PAK_LISTFILE_ENTRY_DELETED) {
            SAFE_FREE(node);
          }
        }
      }
      SAFE_FREE(arhive->listfiles);
      SAFE_FREE(archive);
    }
  __LEAVE_FUNCTION
}

bool isvalid_archivepointer(archive_t *archive) {
  __ENTER_FUNCTION
    if (NULL == arhive) return false;
    if (NULL == arhive->header) return false;
    if (arhive->header.magic != PAK_MAGIC) return false;
    return true;
  __LEAVE_FUNCTION
    return false;
}

void freefile(file_t *&file) {
  __ENTER_FUNCTION
    if (file != NULL) {
      if (file->fp != HANDLE_INVALID) {
        file::closeex(file->fp);
      }
      SAFE_FREE(file->blockoffset);
      SAFE_FREE(file->buffer);
      SAFE_FREE(file);
    }
  __LEAVE_FUNCTION
}

bool isvalid_filepointer(file_t *file) {
  __ENTER_FUNCTION
    if (NULL == file) return false;
    if (NULL == file->fp) return false;
    if (!isvalid_archivepointer(file->archive)) return false;
    return true;
  __LEAVE_FUNCTION
    return false;
}

void freeattributes(attribute_t *attributes) {
  __ENTER_FUNCTION
    if (attributes != NULL) {
      SAFE_FREE(attributes->crc32);
      SAFE_FREE(attributes->filetime);
      SAFE_FREE(attributes->md5);
      SAFE_FREE(attributes);
    }
  __LEAVE_FUNCTION
}

int32_t save_listfile(archive_t *archive, bool internal) {
  __ENTER_FUNCTION
    int32_t error = PAK_ERROR_NONE;
    handle_t fp = HANDLE_INVALID;
    hashkey_t *hashend = NULL, *hash0 = NULL, *hash = NULL;
    uint64_t name1 = 0, name2 = 0;
    filenode_t *node = NULL;
    uint64_t transferred;
    size_t length = 0;
    uint64_t file_savedlength = 1024 * 1024 * 2;
    if (NULL == archive->listfiles) return PAK_ERROR_NONE;
    if (PAK_ERROR_NONE == error) {
      fp = file::createex(LISTFILE_NAME, 
                          GENERIC_READ | GENERIC_WRITE, 
                          0, 
                          NULL, 
                          CREATE_ALWAYS, 
                          FILE_FLAG_DELETE_ON_CLOSE, 
                          NULL);
      if (HANDLE_INVALID == fp) error = get_lasterror();
    }
    char *attributefile_cache = malloc(sizeof(char) * file_savedlength);
    Assert(attributefile_cache);
    if (NULL == attributefile_cache) {
      error = PAK_ERROR_NOT_ENOUGH_MEMORY;
    } else {
      memset(attributefile_cache, 0x00, file_savedlength);
    }
    char *attributefile_compress = 
      malloc(sizeof(char) * (file_savedlength + 8));
    Assert(attributefile_compress);
    if (NULL == attributefile_compress) {
      error = PAK_ERROR_NOT_ENOUGH_MEMORY;
    } else {
      memset(attributefile_compress, 0x00, file_savedlength + 8);
    }
    *(uint64_t *)attributefile_compress = file_savedlength / 2 + 8;
    uint64_t memory_copyed = 0;
    char *copybuffer = attributefile_cache;
    hashend = arhive->hashkey_table + archive->header.hashtable_size;
    hash0 = hash = get_hashentry(archive, NULL);
    if (NULL == hash) hash0 = hash = archive->hashkey_table;
    if (PAK_ERROR_NONE == error) {
      for (;;) {
        if (hash->name1 != name1 && 
            hash->name2 != name2 && 
            hash->blockindex < kBlockStatusHashEntryDeleted) {
          name1 = hash->name1;
          name2 = hash->name2;
          node = archive->listfiles[hash - archive->hashkey_table];
          if ((uint64_t *)node < (uint64_t *)PAK_LISTFILE_ENTRY_DELETED) {
            memory_copyed += node->length + 2;
            if (memory_copyed > file_savedlength) {
              error = PAK_ERROR_INSUFFICIENT_BUFFER;
            } else {
              memcpy(copybuffer, node->filename, node->length);
              copybuffer[node->length + 0] = 0x0D;
              copybuffer[node->length + 1] = 0x0A;
              copybuffer += node->length + 2;
            }
          }
        }
        if (++hash >= hashend) hash = archive->hashkey_table;
        if (hash == hash0) break;
      }
    }
  __LEAVE_FUNCTION
    return -1;
}

int32_t save_listfile(archive_t *archive);
int32_t savetables(archive_t *archive);
uint64_t save_attributefile(archive_t *archive, bool internal);
uint64_t save_attributefile(archive_t *archive);
int32_t addfile_toarchive(archive_t *archive, 
                          void *fp, 
                          const char *archivedname, 
                          uint64_t flag, 
                          uint64_t quality, 
                          int32_t filetype, 
                          bool *replace);
int32_t addfile_toarchive(archive_t *archive, 
                          void *fp, 
                          const char *filename, 
                          uint64_t filelength,
                          const char *archivedname,
                          uint64_t flag, 
                          uint64_t quality, 
                          int32_t filetype, 
                          bool *replace);
int32_t internal_updatefile(archive_t *archive,
                            void *fp, 
                            file_t *file,
                            const char *archivedname,
                            uint64_t flag, 
                            uint64_t quality, 
                            int32_t filetype,
                            bool *replace);

} //namespace util

} //namespace pak

