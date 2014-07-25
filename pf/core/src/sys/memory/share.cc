#include "pf/base/log.h"
#include "pf/base/util.h"
#if __LINUX__
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#elif __WINDOWS__
#include <winbase.h>
#endif
#include "pf/sys/memory/share.h"

namespace pf_sys {

namespace memory { 

namespace share {

uint32_t lock_times = 0;
bool lock_time_enable = false;

//-- struct start
data_header_t::data_header_t() {
  __ENTER_FUNCTION
    key = 0;
    size = 0;
    version = 0;
  __LEAVE_FUNCTION
}

data_header_t::~data_header_t() {
  //do nothing
}
//struct end --

namespace api {

#if __LINUX__
int32_t create(uint64_t key, uint32_t size) {
#elif __WINDOWS__
HANDLE create(uint64_t key, uint32_t size) {
#endif
  __ENTER_FUNCTION
#if __LINUX__
    int32_t handle;
    handle = shmget(key, size, IPC_CREAT | IPC_EXCL | 0666);
    SLOW_ERRORLOG(
        "sharememory",
        "[sys.sharememory] (api::create) handle = %d," 
        " key = %"PRIu64" ,error: %d",
        handle, 
        key, 
        errno);
#elif __WINDOWS__
    HANDLE handle;
    char buffer[65];
    memset(buffer, '\0', sizeof(buffer));
    snprintf(buffer, sizeof(buffer) - 1, "%"PRIu64, key);
    handle = (CreateFileMapping(reinterpret_cast<HANDLE>(0xFFFFFFFFFFFFFFFF), 
                                NULL, 
                                PAGE_READWRITE, 
                                0, 
                                size, 
                                buffer));
#endif
    return handle;
  __LEAVE_FUNCTION
    return NULL;
}
#if __LINUX__
int32_t open(uint64_t key, uint32_t size) {
#elif __WINDOWS__
HANDLE open(uint64_t key, uint32_t size) {
#endif
  __ENTER_FUNCTION
    USE_PARAM(size);
#if __LINUX__
    int32_t handle;
    handle = shmget(key, size, 0);
    SLOW_ERRORLOG(
        "sharememory", 
        "[sys.sharememory] (api::open) handle = %d,"
        " key = %"PRIu64" ,error: %d", 
        handle, 
        key, 
        errno);
#elif __WINDOWS__
    HANDLE handle;
    char buffer[65];
    memset(buffer, '\0', sizeof(buffer));
    snprintf(buffer, sizeof(buffer) - 1, "%"PRIu64, key);
    handle = OpenFileMapping(FILE_MAP_ALL_ACCESS, true, buffer);
#endif
    return handle;
  __LEAVE_FUNCTION
    return NULL;
}

#if __LINUX__
char* map(int32_t handle) {
#elif __WINDOWS__
char* map(HANDLE handle) {
#endif
  __ENTER_FUNCTION
    char* result;
#if __LINUX__
    result = static_cast<char*>(shmat(handle, 0, 0));
#elif __WINDOWS__
    result = 
      static_cast<char*>(MapViewOfFile(handle, FILE_MAP_ALL_ACCESS, 0, 0, 0));
#endif
    return result;
  __LEAVE_FUNCTION
    return NULL;
}

void unmap(char* pointer) {
  __ENTER_FUNCTION
#if __LINUX__
    shmdt(pointer);
#elif __WINDOWS__
    UnmapViewOfFile(pointer);
#endif
  __LEAVE_FUNCTION
}

#if __LINUX__
void close(int32_t handle) {
#elif __WINDOWS__
void close(HANDLE handle) {
#endif
  __ENTER_FUNCTION
#if __LINUX__
    shmctl(handle, IPC_RMID, 0);
#elif __WINDOWS__
    CloseHandle(reinterpret_cast<HANDLE>(handle));
#endif
  __LEAVE_FUNCTION
}

} //namespace api


//-- class start
Base::Base() {
  __ENTER_FUNCTION
    data_pointer_ = NULL;
    handle_ = 0;
    size_ = 0;
    header_ = 0;
  __LEAVE_FUNCTION
}

Base::~Base() {
  //do nothing
}

bool Base::create(uint64_t key, uint32_t size) {
  __ENTER_FUNCTION
    if (kCmdModelClearAll == cmd_model_) return false;
    handle_ = api::create(key, size);
    if (HANDLE_INVALID == handle_) {
      SLOW_ERRORLOG(
          "sharememory", 
          "[sys.sharememory] (Base::create)"
          " failed! handle = %d,key = %"PRIu64"",
          handle_, 
          key);
      return false;
    }
    header_ = api::map(handle_);
    if (header_) {
      data_pointer_ = header_ + sizeof(data_header_t);
      (reinterpret_cast<data_header_t*>(header_))->key = key;
      (reinterpret_cast<data_header_t*>(header_))->size = size;
      size_ = size;
      SLOW_LOG(
          "sharememory", 
          "[sys.sharememory] (Base::create)"
          " success! handle = %d ,key = %"PRIu64"",
          handle_, 
          key);
      return true;
    }
    else {
      SLOW_ERRORLOG(
          "sharememory", 
          "[sys.sharememory] (Base::create)"
          "map failed! handle = %d ,key = %"PRIu64"", 
          handle_, 
          key);
      return false;
    }
  __LEAVE_FUNCTION
    return false;
}

void Base::destory() {
  __ENTER_FUNCTION
    if (header_) {
      api::unmap(header_);
      header_ = NULL;
    }
    if (handle_) {
      api::close(handle_);
      handle_ = NULL;
    }
    size_ = 0;
  __LEAVE_FUNCTION
}

bool Base::attach(uint64_t key, uint32_t size) {
  __ENTER_FUNCTION
    handle_ = api::open(key, size);
    if (kCmdModelClearAll == cmd_model_) {
      destory();
      SLOW_LOG(
          "sharememory",
          "[sys.sharememory] (Base::attach) close memory, key = %"PRIu64"", 
          key);
      return false;
    }
    if (HANDLE_INVALID == handle_) {
      SLOW_ERRORLOG(
          "sharememory", 
          "[sys.sharememory] (Base::attach) failed, key = %"PRIu64"", 
          key); 
      return false;
    }
    header_ = api::map(handle_);
    if (header_) {
      data_pointer_ = header_ + sizeof(data_header_t);
      Assert((reinterpret_cast<data_header_t*>(header_))->key == key);
      Assert((reinterpret_cast<data_header_t*>(header_))->size == size);
      size_ = size;
      SLOW_LOG(
          "sharememory", 
          "[sys.sharememory] (Base::attach) success, key = %"PRIu64"", 
          key); 
      return true;
    }
    else {
      SLOW_ERRORLOG(
          "sharememory", 
          "[sys.sharememory] (Base::attach) map failed, key = %"PRIu64"", 
          key); 
      return false;
    }
  __LEAVE_FUNCTION
    return false;
}

char* Base::get_data_pointer() {
  __ENTER_FUNCTION
    return data_pointer_;
  __LEAVE_FUNCTION
    return NULL;
}

char* Base::get_data(uint32_t size, uint32_t index) {
  __ENTER_FUNCTION
    Assert(size > 0);
    Assert(size * index < size_);
    char* result;
    result = (size <= 0 || index > size_) ? NULL : data_pointer_ + size * index;
    return result;
  __LEAVE_FUNCTION
    return NULL;
}

uint32_t Base::get_size() {
  __ENTER_FUNCTION
    return size_;
  __LEAVE_FUNCTION
    return 0;
}

bool Base::dump(const char* filename) {
  __ENTER_FUNCTION
    Assert(filename);
    FILE* fp = fopen(filename, "wb");
    if (!fp) return false;
    fwrite(header_, 1, size_, fp);
    fclose(fp);
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Base::merge_from_file(const char* filename) {
  __ENTER_FUNCTION
    Assert(filename);
    FILE* fp = fopen(filename, "rb");
    if (!fp) return false;
    fseek(fp, 0L, SEEK_END);
    int32_t filelength = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    fread(header_, filelength, 1, fp);
    fclose(fp);
    return true;
  __LEAVE_FUNCTION
    return false;
}

void Base::set_head_version(uint32_t version) {
  __ENTER_FUNCTION
    (reinterpret_cast<data_header_t*>(header_))->version = version;
  __LEAVE_FUNCTION
}

uint32_t Base::get_head_version() {
  __ENTER_FUNCTION
    uint32_t version = (reinterpret_cast<data_header_t*>(header_))->version;
    return version;
  __LEAVE_FUNCTION
    return 0;
}

//class end --

//-- functions start

void lock(char &flag, char type) {
  __ENTER_FUNCTION
    _loop:
    if (kUseFree == flag) {
      flag = type;
      if (flag != type) {
        pf_base::util::sleep(1);
#if __LINUX__
        ++lock_times;
        printf("[sys.sharememory] (lock) fail %s, %d, %s", 
               __FILE__, 
               __LINE__, 
               __PRETTY_FUNCTION__);
        if (lock_times < 100 && lock_time_enable) {
          lock_times = 0;
          return;
        }
#endif
        goto _loop;
      }
    }
    else {
      goto _loop;
    }
  __LEAVE_FUNCTION
}

bool trylock(char &flag, char type) {
  __ENTER_FUNCTION
    uint32_t _lock_times = 0;
    _loop:
    if (kUseFree == flag) {
      flag = type;
      if (flag != type) {
        ++_lock_times;
        pf_base::util::sleep(1);
        if (_lock_times > 10) {
          return false;
        }
        goto _loop;
      }
    }
    else {
      ++_lock_times;
      pf_base::util::sleep(1);
      if (_lock_times > 10) {
        return false;
      }
      goto _loop;
    }
  __LEAVE_FUNCTION
    return false;
}

void unlock(char &flag, char type) {
  __ENTER_FUNCTION
    USE_PARAM(type);
    _loop:
      if (kUseFree == flag) return;
      flag = kUseFree;
      if (kUseFree != flag) {
        pf_base::util::sleep(1);
#if __LINUX__
        printf("[sys.sharememory] (unlock) failed %s, %d, %s", 
               __FILE__, 
               __LINE__, 
               __PRETTY_FUNCTION__);
        if (lock_times > 100 && lock_time_enable) {
          lock_times = 0;
          return;
        }
#endif
        goto _loop;
      }
  __LEAVE_FUNCTION
}

//functions end --

} //namespace share

} //namespace share

} //namespace pf_sys
