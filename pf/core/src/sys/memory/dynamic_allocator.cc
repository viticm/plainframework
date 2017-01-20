#include "pf/basic/logger.h"
#include "pf/sys/memory/dynamic_allocator.h"

namespace pf_sys {

namespace memory {

DynamicAllocator::DynamicAllocator() {
  pointer_ = NULL;
  size_ = 0;
}

DynamicAllocator::~DynamicAllocator() {
  free();
}

void *DynamicAllocator::malloc(size_t size) {
  if (size_ == size) return pointer_;
  void *pointer = reinterpret_cast<void *>(new char[size]);
  if (is_null(pointer)) {
    Assert(false);
    return nullptr;
  }
  memset(pointer, 0, size);
  if (!is_null(pointer_)) {
    size_t copysize = size > size_ ? size_ : size;
    memcpy(pointer, pointer_, copysize);
    free();
  }
  pointer_ = pointer;
  size_ = size;
  return pointer_;
}

size_t DynamicAllocator::size() const {
  return size_;
}

void DynamicAllocator::free() {
  char *pointer = reinterpret_cast<char*>(pointer_);
  if (!is_null(pointer)) {
    delete[] pointer;
    pointer_ = nullptr;
  }
  size_ = 0;
}

void *DynamicAllocator::get() {
  return pointer_;
}

}; //namespace memory

}; //namespace pf_sys
