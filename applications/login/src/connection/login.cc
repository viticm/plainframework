#include "pf/base/time_manager.h"
#include "pf/base/log.h"
#include "common/define/enum.h"
#include "common/net/packet/login_toclient/turnstatus.h"
#include "connection/queue/turn.h"
#include "connection/login.h"

namespace connection {

Login::Login() {
  __ENTER_FUNCTION
    clear();
  __LEAVE_FUNCTION
}

Login::~Login() {
  //do nothing
}

bool Login::init() {
  __ENTER_FUNCTION
    resetkick();
    if (!Base::init()) return false;
    return true;
  __LEAVE_FUNCTION
    return false;
}

void Login::resetkick() {
  __ENTER_FUNCTION
    kicktime_ = TIME_MANAGER_POINTER->get_current_time();
  __LEAVE_FUNCTION
}

void Login::clear() {
  __ENTER_FUNCTION
    memset(account_, 0, sizeof(account_));
    rolenumber_ = 0;
    status_ = kPlayerStatusLoginEmpty;
    kicktime_ = 0;
    connecttime_ = 0;
    gatewaytime_ = 0;
    last_sendmessage_turntime_ = 0;
    readykick_count_ = 0;
  __LEAVE_FUNCTION
}

bool Login::isplayer() const {
  return true;
}

bool Login::isserver() const {
  return false;
}

bool Login::heartbeat(uint32_t time) {
  __ENTER_FUNCTION
    using namespace common::net::packet::login_toclient;
    if (time > kicktime_ + CONNECTION_KICKTIME_MAX) {
      SLOW_ERRORLOG(APPLICATION_NAME,
                    "[connection] (Login::heartbeat)"
                    " didn't receive message for long time. Kicked!");
      return false;
    }
    if (getstatus() == kPlayerStatusLoginProcessTurn) {
      if (time > last_sendmessage_turntime_ + CONNECTION_TURNMESSAGE_TIME_MAX) {
        last_sendmessage_turntime_ = time;
        uint32_t turnnumber = 0;
        turnnumber = CONNECTION_QUEUE_TURN_POINTER->calculate_turnnumber(queueposition_);
        TurnStatus message;
        message.set_turnstatus(kPlayerStatusLoginProcessTurn);
        message.set_turnnumber(turnnumber);
        sendpacket(&message);
      }
    }
    if (!Base::heartbeat(time)) return false;
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Login::flushdirectly(pf_net::packet::Base *packet) {
  __ENTER_FUNCTION
    bool result = Base::sendpacket(packet);
    if (!result || !Base::processoutput()) return false;
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Login::senddirectly(pf_net::packet::Base *packet) {
  __ENTER_FUNCTION
    bool result = Base::sendpacket(packet);
    if (result) {
      int8_t count = 0;
      while (socket_inputstream_->reallength() > 0) {
        Base::processoutput();
        ++count;
        if (count > 3) {
          result = false;
          break;
        }
      }
    }
    return result;
  __LEAVE_FUNCTION
    return false;
}

uint32_t Login::get_connecttime() const {
  return connecttime_;
}

void Login::set_connecttime(uint32_t time) {
  connecttime_ = time;
}

uint32_t Login::get_readykick_count() const {
  return readykick_count_;
}

void Login::set_readykick_count(uint32_t count) {
  readykick_count_ = count;
}

int64_t Login::getguid(int8_t index) const {
  __ENTER_FUNCTION
    int64_t guid = role_baselist_[index].guid;
    return guid;
  __LEAVE_FUNCTION
    return ID_INVALID;
}

void Login::set_queueposition(uint16_t position) {
  queueposition_ = position;
}
   
uint16_t Login::get_queueposition() const {
  return queueposition_;
}

void Login::set_gatewaytime(uint32_t time) {
  gatewaytime_ = time;
}

uint32_t Login::get_gatewaytime() const {
  return gatewaytime_;
}

void Login::set_last_sendmessage_turntime(uint32_t time) {
  last_sendmessage_turntime_ = time;
}

uint32_t Login::get_last_sendmessage_turntime() const {
  return last_sendmessage_turntime_;
}
   
void Login::set_role_baseinfo(int64_t guid, uint8_t level, int8_t index) {
  __ENTER_FUNCTION
    Assert(index >= 0 && index <= DB_ROLE_MAX);
    role_baselist_[index].guid = guid;
    role_baselist_[index].level = level;
  __LEAVE_FUNCTION
}

bool Login::is_guidowner(int64_t guid) const {
  __ENTER_FUNCTION
    uint8_t i;
    for (i = 0; (i < rolenumber_) && (i < DB_ROLE_MAX); ++i) {
      if (role_baselist_[i].guid == guid && guid != ID_INVALID) return true;
    }
    return false;
  __LEAVE_FUNCTION
    return false;
}
   
uint8_t Login::get_rolelevel(int64_t guid) const {
  __ENTER_FUNCTION
    uint8_t i;
    for (i = 0; (i < rolenumber_) && (i < DB_ROLE_MAX); ++i) {
      if (role_baselist_[i].guid == guid && guid != ID_INVALID) 
        return role_baselist_[i].level;
    }
    return 1;
  __LEAVE_FUNCTION
    return 1;
}

const char *Login::getaccount() {
  return account_;
}

} //namespace connection
