#include "pf/basic/util.h"
#include "pf/basic/string.h"
#include "pf/basic/logger.h"
#include "pf/script/lua/filebridge.h"

#ifdef PF_OPEN_LUA

using namespace pf_basic;

namespace pf_script {

namespace lua {

FileBridge::FileBridge() : 
  fp_{nullptr},
  length_{0},
  position_{0},
  rootpath_{0},
  workpath_{0} {
  string::safecopy(rootpath_, SCRIPT_ROOT_PATH, sizeof(rootpath_));
  string::safecopy(workpath_, SCRIPT_WORK_PATH, sizeof(workpath_));
}

bool FileBridge::open(const char *filename) {
  if (fp_) close();
  char filepath[FILENAME_MAX] = {0};
  get_fullpath(filepath, filename, sizeof(filepath) - 1);
#if OS_WIN
  util::path_tounix(filepath, sizeof(filepath) - 1);
  char casepath[FILENAME_MAX] = {0};
  string::safecopy(casepath, filepath, sizeof(casepath));
  fp_ = fopen(casepath, "rb");
#endif
  fp_ = fp_ ? fp_ : fopen(filepath, "rb");
  if (nullptr == fp_) {
    SLOW_ERRORLOG(SCRIPT_MODULENAME, 
                  "[script][lua] (FileBridge::open) file error: %s", 
                  filepath);  
    return false;
  }
  return true;
}

void FileBridge::close() {
  if (fp_) fclose(fp_);
  fp_ = nullptr;
  length_ = 0;
  position_ = 0;
}

uint64_t FileBridge::read(void *buffer, uint64_t read_bytes) {
  if (!fp_) return 0;
  uint64_t _read_bytes;
  _read_bytes = fread(buffer, 1, static_cast<size_t>(read_bytes), fp_);  
  position_ += _read_bytes;
  return _read_bytes;
}

uint64_t FileBridge::write(void *buffer, uint64_t write_bytes) {
  if (!fp_) return 0;
  uint64_t _write_bytes;
  _write_bytes = fwrite(buffer, 1, static_cast<size_t>(write_bytes), fp_); 
  position_ += _write_bytes;
  return _write_bytes;
}

int64_t FileBridge::seek(int64_t position, file_accessmode_t accessmode) {
  if (!fp_) return -1;
  fseek(fp_, static_cast<long>(position), accessmode);
  position_ = ftell(fp_);
  return position_;
}

uint64_t FileBridge::size() {
  if (!fp_) return 0;
  if (0 == length_) {
    uint64_t temp = position_;
    length_ = static_cast<uint64_t>(seek(0, kFileAccessModeEnd));
    seek(temp, kFileAccessModeBegin);
  }
  return length_;
}

void FileBridge::get_fullpath(char *path, 
                              const char *filename, 
                              size_t length) {
  if (':' == filename[1]) {
    string::safecopy(path, filename, length);
  }
  if ('\\' == filename[0] || '/' == filename[0]) {
    string::safecopy(path, rootpath_, length);
    strncat(path, filename, length - strlen(path));
    return;
  }
#if OS_WIN
  if (':' == workpath_[1]) {
    string::safecopy(path, workpath_, length);
    strncat(path, filename, length - strlen(path));
    return;
  }
#endif 
  string::safecopy(path, rootpath_, length);
  if (workpath_[0] != '\\' && workpath_[0] != '/') {
#if OS_WIN /* { */
    strncat(path, "\\", length - strlen(path)); 
#elif OS_UNIX /* }{ */
    strncat(path, "/", length - strlen(path)); 
#endif /* } */
  }
  strncat(path, workpath_, length - strlen(path));
  if ('.' == filename[0] && ('\\' == filename[1] || '/' == filename[1])) {
    strncat(path, filename + 2, length - strlen(path));      
  }
  else {
    strncat(path, filename, length - strlen(path));
  }
}

} //namespace lua

} //namespace pf_script

#endif //PF_OPEN_LUA
