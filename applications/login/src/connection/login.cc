#include "pf/base/time_manager.h"
#include "pf/base/log.h"
#include "common/define/enum.h"
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
    characternumber_ = 0;
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
      while (outputstream_->reallength() > 0) {
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

} //namespace connection
