#include "pf/basic/logger.h"
#include "pf/basic/util.h"
#if OS_UNIX
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#elif OS_WIN
#include <winbase.h>
#endif
#include "pf/sys/memory/share.h"

namespace pf_sys {

namespace memory { 

namespace share {

header_struct::header_struct() :
  key{0}, 
  size{0},
  version{0},
  pool_position{0},
  mutex{kFlagFree} {

}                                                                                  

void header_struct::clear() {
  key = 0;                                                                       
  size = 0;                                                                      
  mutex.exchange(kFlagFree);
  version = 0;                                                                   
  pool_position = 0;
}

void header_struct::lock(int8_t type) {
  share::lock(mutex, type);
}
  
void header_struct::unlock(int8_t type) {
  share::unlock(mutex, type);
}
                                                                                  
header_struct::~header_struct() {                                          
  //do nothing
}

dataheader_struct::dataheader_struct() {
  clear();
}

dataheader_struct::~dataheader_struct() {
  //do nothing
}

void dataheader_struct::clear() {
  key = 0;
  version = 0;
  usestatus = kUseFree;
  pool_id = ID_INVALID;
  mutex.exchange(kFlagFree);
}

  
dataheader_t &dataheader_struct::operator = (const dataheader_t &object) {
  key = object.key;
  version = object.version;
  int8_t flag = object.mutex;
  mutex.exchange(flag);
  pool_id = object.pool_id;
  return *this;
}
  
dataheader_t *dataheader_struct::operator = (const dataheader_t *object) {
  if (object) {
    key = object->key;
    version = object->version;
    int8_t flag = object->mutex;
    mutex.exchange(flag);
    pool_id = object->pool_id;

  }
  return this;
}
 
//struct end --

namespace api {

#if OS_UNIX
int32_t create(uint32_t key, size_t size) {
  int32_t handle = 0;
#elif OS_WIN
HANDLE create(uint32_t key, size_t size) {
    HANDLE handle = nullptr;
#endif
#if OS_UNIX
    handle = shmget(key, size, IPC_CREAT | IPC_EXCL | 0666);
    if (HANDLE_INVALID == handle) {
      SLOW_ERRORLOG(
          GLOBALS["app.name"].string(),
          "[sys.memory.share] (api::create) handle = %d," 
          " key = %d, error: %d",
          handle, 
          key, 
          errno);
    }
#elif OS_WIN
    char buffer[65]{0,};
    snprintf(buffer, sizeof(buffer) - 1, "%d", key);
    handle = (CreateFileMapping(reinterpret_cast<HANDLE>(0xFFFFFFFFFFFFFFFF), 
                                NULL, 
                                PAGE_READWRITE, 
                                0, 
                                size, 
                                buffer));
#endif
    return handle;
}
#if OS_UNIX
int32_t open(uint32_t key, size_t size) {
  int32_t handle = 0;
#elif OS_WIN
HANDLE open(uint32_t key, size_t size) {
  HANDLE handle = nullptr;
#endif
#if OS_UNIX
    handle = shmget(key, size, 0);
    if (HANDLE_INVALID == handle) {
      SLOW_ERRORLOG(
          GLOBALS["app.name"].string(), 
          "[sys.memory.share] (api::open) handle = %d,"
          " key = %d, error: %d", 
          handle, 
          key, 
          errno);
    }
#elif OS_WIN
    char buffer[65];
    memset(buffer, '\0', sizeof(buffer));
    snprintf(buffer, sizeof(buffer) - 1, "%d", key);
    handle = OpenFileMapping(FILE_MAP_ALL_ACCESS, true, buffer);
#endif
    return handle;
}

#if OS_UNIX
char *map(int32_t handle) {
#elif OS_WIN
char *map(HANDLE handle) {
#endif
    char *result;
#if OS_UNIX
    result = static_cast<char *>(shmat(handle, 0, 0));
#elif OS_WIN
    result = 
      static_cast<char *>(MapViewOfFile(handle, FILE_MAP_ALL_ACCESS, 0, 0, 0));
#endif
    return result;
    return NULL;
}

void unmap(char *pointer) {
#if OS_UNIX
    shmdt(pointer);
#elif OS_WIN
    UnmapViewOfFile(pointer);
#endif
}

#if OS_UNIX
void close(int32_t handle) {
#elif OS_WIN
void close(HANDLE handle) {
#endif
#if OS_UNIX
    shmctl(handle, IPC_RMID, 0);
#elif OS_WIN
    CloseHandle(reinterpret_cast<HANDLE>(handle));
#endif
}

} //namespace api


//-- class start
Base::Base() :
  size_{0},
  data_{nullptr},
  header_{nullptr},
  handle_{HANDLE_INVALID} {
}

Base::~Base() {
  //do nothing
}

bool Base::create(uint32_t key, size_t size) {
    if (GLOBALS["app.cmdmodel"] == kCmdModelClearAll) return false;
    handle_ = api::create(key, size);
    if (HANDLE_INVALID == handle_) {
      SLOW_ERRORLOG(
          GLOBALS["app.name"].string(), 
          "[sys.memory.share] (Base::create)"
          " failed! handle = %d, key = %d",
          handle_, 
          key);
      return false;
    }
    header_ = api::map(handle_);
    if (header_) {
      data_ = header_ + sizeof(header_t);
      header()->clear();
      header()->key = key;
      header()->size = size;
      size_ = size;
      SLOW_LOG(
          GLOBALS["app.name"].string(), 
          "[sys.memory.share] (Base::create)"
          " success! handle = %d, key = %d",
          handle_, 
          key);
      return true;
    } else {
      SLOW_ERRORLOG(
          GLOBALS["app.name"].string(), 
          "[sys.memory.share] (Base::create)"
          "map failed! handle = %d, key = %d", 
          handle_, 
          key);
      return false;
    }
    return false;
}

void Base::release() {
    if (header_) {
      api::unmap(header_);
      header_ = NULL;
    }
    if (handle_) {
      api::close(handle_);
#if OS_UNIX
      handle_ = 0;
#elif OS_WIN
      handle_ = NULL;
#endif
    }
    size_ = 0;
}

bool Base::attach(uint32_t key, size_t size) {
    handle_ = api::open(key, size);
    if (GLOBALS["app.cmdmodel"] == kCmdModelClearAll) {
      release();
      SLOW_LOG(
          GLOBALS["app.name"].string(),
          "[sys.memory.share] (Base::attach) close memory, key = %d", 
          key);
      return false;
    }
    if (HANDLE_INVALID == handle_) {
      SLOW_ERRORLOG(
          GLOBALS["app.name"].string(), 
          "[sys.memory.share] (Base::attach) failed, key = %d", 
          key); 
      return false;
    }
    header_ = api::map(handle_);
    if (header_) {
      data_ = header_ + sizeof(header_t);
      Assert(header()->key == key);
      Assert(header()->size == size);
        size_ = size;
      SLOW_LOG(
          GLOBALS["app.name"].string(), 
          "[sys.memory.share] (Base::attach) success, key = %d", 
          key); 
      return true;
    } else {
      SLOW_ERRORLOG(
          GLOBALS["app.name"].string(), 
          "[sys.memory.share] (Base::attach) map failed, key = %d", 
          key); 
      return false;
    }
    return true;
}

char *Base::get(uint32_t index, size_t size) {
    Assert(size > 0);
    Assert(size * index < size_);
    char *result;
    result = 
      (size <= 0 || size * index > size_) ? NULL : data_ + size * index;
    return result;
}

bool Base::dump(const char *filename) {
    Assert(filename);
    FILE* fp = fopen(filename, "wb");
    if (!fp) return false;
    fwrite(header_, 1, size_, fp);
    fclose(fp);
    return true;
}

bool Base::merge(const char *filename) {
    Assert(filename);
    FILE *fp = fopen(filename, "rb");
    if (!fp) return false;
    fseek(fp, 0L, SEEK_END);
    int32_t filelength = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    fread(header_, filelength, 1, fp);
    fclose(fp);
    return true;
}

//class end --

//-- functions start

void lock(mutex_t &mutex, int8_t type) {
  if (GLOBALS["app.cmdmodel"] == kCmdModelRecover ||
      GLOBALS["app.status"] == kAppStatusStop) return;
  int32_t count = 0;
  int8_t flag = 0;
  while (!mutex.compare_exchange_weak(flag, kFlagFree)) {
    if (GLOBALS["app.status"] == kAppStatusStop) break;
    ++count;
    std::this_thread::sleep_for(std::chrono::milliseconds(0));
    if (count > 100) {
      char time_str[256] = {0};
      pf_basic::Logger::get_log_timestr(time_str, sizeof(time_str) - 1);
      pf_basic::io_cerr("%s[sys.memory] (share::lock) failed", time_str);
      count = 0;
    }
  }
}

void unlock(mutex_t &mutex, int8_t type) {
  if (GLOBALS["app.cmdmodel"] == kCmdModelRecover ||
      GLOBALS["app.status"] == kAppStatusStop) return;
  int8_t flag = kFlagFree;
  int8_t count{0};
  while (!mutex.compare_exchange_weak(flag, 0)) {
    if (GLOBALS["app.status"] == kAppStatusStop) break;
    ++count;
    std::this_thread::sleep_for(std::chrono::milliseconds(0));
    if (count > 100) {
      mutex.exchange(kFlagFree);
      char time_str[256] = {0};
      pf_basic::Logger::get_log_timestr(time_str, sizeof(time_str) - 1);
      pf_basic::io_cerr("%s[sys.memory] (share::lock) failed", time_str);
      count = 0;
    }
  }
}

GroupPool::GroupPool(uint32_t key, const std::vector<group_item_t> &group) :
  key_{key},
  size_{0} {
  size_ += sizeof(group_header_t);
  for (size_t i = 0; i < group.size(); ++i) {
    const group_item_t &item = group[i];
    group_item_t temp;
    temp.index = item.index;
    temp.position = size_;
    temp.size = item.size;
    temp.header_size = item.header_size;
    temp.data_size = item.data_size;
    temp.same_header = item.same_header;
    group_conf_[item.index] = temp;
    size_t size = 0;
    if (item.same_header) {
      size = sizeof(group_item_header_t) + 
             item.header_size + 
             item.data_size * item.size;
    } else {
      size = sizeof(group_item_header_t) +
             (item.header_size + item.data_size) * size;
    }
    size_ += size; 
  }
}

bool GroupPool::init(bool create) {
  if (0 == key_ || 0 == size_) return false;
  if (ready_) return true;
  std::unique_ptr<Base> ptr(new Base());
  ref_obj_pointer_ = std::move(ptr);
  Assert(ref_obj_pointer_);
  if (is_null(ref_obj_pointer_)) return false;
  bool result = true;
  bool need_init = false;
  result = ref_obj_pointer_->attach(key_, size_);
  if (create && !result) {
    result = ref_obj_pointer_->create(key_, size_);
    need_init = true;
  } else if (!result) {
    return false;
  }
  if (!result && GLOBALS["app.cmdmodel"] == kCmdModelClearAll) {
    return true;
  } else if (!result) {
    SLOW_ERRORLOG(
        "sharememory",
        "[sys][sharememory] (GroupPool::init) failed");
    Assert(result);
    return false;
  }

  if (need_init) {
    char *data = ref_obj_pointer_->get();
    memset(data, 0, size_);
  }
  ready_ = true;
  return true;
}

char *GroupPool::get_data(int16_t index) {
  const group_item_t &item = group_conf_[index];
  return ref_obj_pointer_->get() + sizeof(group_header_t) + item.position;
}

group_header_t *GroupPool::header() {
  return reinterpret_cast<group_header_t *>(ref_obj_pointer_->get());
}
   
char *GroupPool::item_data_header(int16_t index, int16_t data_index) {
  char *data = get_data(index);
  const group_item_t &item = group_conf_[index];
  char *result = nullptr;
  size_t header_size = sizeof(group_item_header_t);
  if (static_cast<size_t>(data_index) >= item.size) return nullptr;
  if (item.same_header) {
    result = data + header_size;
  } else {
    result = 
      data + header_size + (item.header_size + data_index) * item.data_size;
  }
  return result;
}

group_item_header_t *GroupPool::item_header(int16_t index) {
  char *data = get_data(index);
  return reinterpret_cast<group_item_header_t *>(data);
}
   
char *GroupPool::item_data(int16_t index, int16_t data_index) {
  char *data = get_data(index);
  const group_item_t &item = group_conf_[index];
  char *result = nullptr;
  if (item.same_header) {
    result = data + 
             item.header_size + 
             data_index * item.data_size + 
             sizeof(group_item_header_t);
  } else {
    if (static_cast<size_t>(data_index) >= item.size) return nullptr;
    result = data + 
           (item.header_size + data_index) * item.data_size + 
           sizeof(group_item_header_t);
  }
  return result;
}

void GroupPool::free() {
  for (size_t i = 0; i < group_conf_.size(); ++i) {
    const group_item_t &item = group_conf_[i];
    group_item_header_t *header = item_header(item.index);
    unique_lock< group_item_header_t > auto_lock(*header, kFlagMixedWrite);
    header->pool_position = 0;
  }
}

char *GroupPool::alloc(int16_t index, int16_t &data_index) {
  group_item_header_t *header = item_header(index);
  Assert(header);
  unique_lock<group_item_header_t> auto_lock(*header, kFlagMixedWrite);
  char *data = get_data(index);
  const group_item_t &item = group_conf_[index];
  char *result = nullptr;
  if (static_cast<size_t>(header->pool_position) >= item.size) return result;
  data_index = header->pool_position;
  if (static_cast<size_t>(data_index) >= item.size) return nullptr;
  if (item.same_header) {
    result = data + 
             item.header_size + 
             data_index * item.data_size + 
             sizeof(group_item_header_t);
    memset(result, 0, item.data_size);
  } else {
    result = data + 
           (item.header_size + item.data_size) * data_index + 
           sizeof(group_item_header_t);
    memset(result, 0, item.data_size + item.header_size);
  }
  header->pool_position += 1;
  return result;
}

int16_t GroupPool::free(int16_t index, int16_t data_index) {
  group_item_header_t *header = item_header(index);
  Assert(header);
  unique_lock<group_item_header_t> auto_lock(*header, kFlagMixedWrite);
  const group_item_t &item = group_conf_[index];
  if (data_index >= header->pool_position) return INDEX_INVALID;
  --(header->pool_position);
  char *swap_data = item_data(index, header->pool_position);
  size_t data_size = sizeof(group_item_header_t) + item.data_size;
  if (!item.same_header) {
    data_size += item.header_size;
  }
  char *delete_data = item_data(index, data_index);
  memcpy(delete_data, swap_data, data_size);
  return header->pool_position;
}

//functions end --

} //namespace share

} //namespace share

} //namespace pf_sys
