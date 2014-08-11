#include "connection/queue/center.h"

namespace connection {

namespace queue {

Center::Center() {
  __ENTER_FUNCTION
    size_ = CONNECTION_QUEUE_CENTER_TURN_MAX;
    queue_ = new centerinfo_t[size_];
    head_ = tail_ = 0;
  __LEAVE_FUNCTION
}

Center::~Center() {
  __ENTER_FUNCTION
    SAFE_DELETE_ARRAY(queue_);
  __LEAVE_FUNCTION
}

bool Center::init() {
  return true;
}

bool Center::addin(int16_t id, 
                   const char *name, 
                   int64_t guid, 
                   uint16_t &queueposition) {
  __ENTER_FUNCTION
    using namespace pf_sys;
    lock_guard<ThreadLock> autolock(lock_); //自动锁
    if (queue_[queueposition].isused) return false;
    queue_[queueposition].isused = true;
    queue_[queueposition].id = id;
    pf_base::safecopy(queue_[queueposition].name, 
                      name, 
                      sizeof(queue_[queueposition].name));
    queue_[queueposition].guid = guid;
    queue_[queueposition].queueposition = tail_;
    ++tail_;
    if (tail_ > size_) tail_ = 0;
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Turn::getout(int16_t &id, char *name) {
  __ENTER_FUNCTION
    using namespace pf_sys;
    if (false == queue_[head_].isused) return false;
    Assert(name);
    pf_base::safecopy(name, queue_[head_].name, ACCOUNT_LENGTH_MAX);
    id = queue_[head_].id;
    queue_[head_].isused = false;
    ++head_;
    if (head_ >= size_) head_ = 0;
    return true;
  __LEAVE_FUNCTION
    return false;
}

centerinfo_t &Center::get(uint16_t queueposition) {
  return queue_[queueposition];
}

bool Center::isempty() const {
  bool result = tail_ == head_;
  return result;
}

uint16_t Center::getcount() const {
  uint16_t result = 0;
  if (head_ < tail_) {
    result = tail_ - head_;
  } else {
    result = size_ - (head_ - tail_);
  }
  return result;
}

uint16_t Center::gethead() const {
  return head_;
}

} //namespace queue

} //namespace connection
