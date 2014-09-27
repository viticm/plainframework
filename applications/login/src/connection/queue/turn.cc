#include "pf/base/string.h"
#include "pf/net/connection/pool.h"
#include "common/define/enum.h"
#include "engine/system.h"
#include "connection/manager/login.h"
#include "connection/login.h"
#include "connection/queue/turn.h"

connection::queue::Turn *g_connection_queue_turn = NULL;

template <>
connection::queue::Turn 
  *pf_base::Singleton<connection::queue::Turn>::singleton_ = NULL;

namespace connection {

namespace queue {

Turn::Turn() {
  __ENTER_FUNCTION
    size_ = CONNECTION_QUEUE_TURN_MAX;
    queue_ = new turninfo_t[size_];
    Assert(queue_);
    head_ = tail_ = 0;
  __LEAVE_FUNCTION
}

Turn::~Turn() {
  __ENTER_FUNCTION
    SAFE_DELETE_ARRAY(queue_);
  __LEAVE_FUNCTION
}

Turn *Turn::getsingleton_pointer() {
  return singleton_;
}

Turn &Turn::getsingleton() {
  Assert(singleton_);
  return *singleton_;
}

bool Turn::init() {
  return true;
}

bool Turn::addin(int16_t id, const char *name, uint16_t &queueposition) {
  __ENTER_FUNCTION
    using namespace pf_sys;
    lock_guard<ThreadLock> autolock(lock_); //自动锁
    if (queue_[queueposition].isused) return false;
    queue_[queueposition].isused = true;
    queue_[queueposition].id = id;
    pf_base::string::safecopy(queue_[queueposition].name, 
                              name, 
                              sizeof(queue_[queueposition].name));
    queue_[queueposition].queueposition = tail_;
    ++tail_;
    if (tail_ > size_) tail_ = 0;
    return true;
  __LEAVE_FUNCTION
    return false;
}

void Turn::erase(const char *name, int16_t id) {
  __ENTER_FUNCTION
    Assert(name);
    uint16_t i;
    pf_net::connection::Pool *connectionpool = 
      ENGINE_SYSTEM_POINTER->get_netmanager()->getpool();
    Assert(connectionpool);
    for (i = 0; i < size_; ++i) {
      lock_.lock();
      if (queue_[i].isused) {
        connection::Login *loginconnection = 
          dynamic_cast<connection::Login *>(connectionpool->get(queue_[i].id));
        if (loginconnection) {
          if (kConnectionStatusLoginProcessTurn == loginconnection->getstatus() && 
              loginconnection->getid() != id && //不删除自己
              0 == strcmp(loginconnection->getaccount(), name)) {
            CONNECTION_MANAGER_LOGIN_POINTER->remove(loginconnection);
            break;
          }
        }
      }
      lock_.unlock();
    }
  __LEAVE_FUNCTION
}

bool Turn::getout(int16_t &id, char *name) {
  __ENTER_FUNCTION
    using namespace pf_sys;
    if (false == queue_[head_].isused) return false;
    Assert(name);
    pf_base::string::safecopy(name, queue_[head_].name, ACCOUNT_LENGTH_MAX);
    id = queue_[head_].id;
    queue_[head_].isused = false;
    ++head_;
    if (head_ >= size_) head_ = 0;
    return true;
  __LEAVE_FUNCTION
    return false;
}

uint16_t Turn::calculate_turnnumber(uint16_t queueposition) const {
  __ENTER_FUNCTION
    uint16_t i = 0;
    uint16_t turnnumber = 1;
    uint16_t head = gethead();
    pf_net::connection::Pool *connectionpool = 
      ENGINE_SYSTEM_POINTER->get_netmanager()->getpool();
    Assert(connectionpool);
    if (head <= queueposition) {
      for (i = head; i < queueposition; ++i) {
        connection::Login *loginconnection = dynamic_cast<connection::Login *>(
            connectionpool->get(queue_[i].id));
        if (loginconnection && 
            kConnectionStatusLoginProcessTurn == loginconnection->getstatus()) {
          ++turnnumber;
        }
      }
    } else {
      for (i = head; i < CONNECTION_QUEUE_TURN_MAX; ++i) {
        connection::Login *loginconnection = dynamic_cast<connection::Login *>(
            connectionpool->get(queue_[i].id));
        if (loginconnection &&
            kConnectionStatusLoginProcessTurn == loginconnection->getstatus()) {
          ++turnnumber;
        }
      }
      for (i = 0; i < queueposition; ++i) {
        connection::Login *loginconnection = dynamic_cast<connection::Login *>(
            connectionpool->get(queue_[i].id));
        if (loginconnection && 
            kConnectionStatusLoginProcessTurn == loginconnection->getstatus()) { 
          ++turnnumber;
        }
      }
    }
    return turnnumber;
  __LEAVE_FUNCTION
    return 0;
}

turninfo_t &Turn::get(uint16_t queueposition) {
  return queue_[queueposition];
}

bool Turn::isempty() const {
  bool result = tail_ == head_;
  return result;
}

uint16_t Turn::getcount() const {
  uint16_t result = 0;
  if (head_ < tail_) {
    result = tail_ - head_;
  } else {
    result = (size_ - (head_ - tail_));
  }
  return result;
}

uint16_t Turn::gethead() const {
  return head_;
}

} //namespace queue

} //namespace connection
