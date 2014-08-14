#include "pf/base/util.h"
#include "pf/base/log.h"
#include "pf/base/time_manager.h"
#include "common/define/enum.h"
#include "common/setting.h"
#include "common/net/packet/login_toclient/turnstatus.h"
#include "connection/queue/turn.h"
#include "connection/counter/center.h"
#include "connection/login.h"
#include "engine/thread/net/login.h"

using namespace engine::thread::net;
using namespace common::net::packet;

Login::Login() {
  //do nothing
}

Login::~Login() {
  //do nothing
}

bool Login::init() {
  __ENTER_FUNCTION
    isactive_ = connection::manager::Login::init(
        SETTING_POINTER->login_info_.net_connectionmax);
    return isactive_;
  __LEAVE_FUNCTION
    return false;
}

void Login::run() {
  __ENTER_FUNCTION
    while (isactive()) {
      bool result = false;
      pf_base::util::sleep(10);
      uint32_t currenttime = TIME_MANAGER_POINTER->get_current_time();
      result = dotick(currenttime);
      Assert(result);
#ifdef _EXEONECE
      static int32_t count = _EXEONECE;
      --count;
      if (count <= 0) {
        SLOW_LOG(NET_MODULENAME,
                 "[engine.thread.net] (Login::run) process will exit."
                 " thread id: %"PRIu64""
                 pf_sys::get_current_thread_id());
        break;
      }
#endif
    }
    quit();
  __LEAVE_FUNCTION
}

void Login::stop() {
  __ENTER_FUNCTION
    isactive_ = false;
  __LEAVE_FUNCTION
}

void Login::quit() {
  //do nothing
}

bool Login::isactive() const {
  return isactive_;
}

bool Login::dotick(uint32_t time) {
  __ENTER_FUNCTION
    bool result = false;
    connection::manager::Login::tick();
    try {
      result = move_queueplayer();
      AssertEx(result, "move_queueplayer error");
    } catch (...) {

    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Login::move_queueplayer() {
  __ENTER_FUNCTION
    uint16_t count = connection::manager::Login::getcount();
    int16_t connectionid;
    char name[ACCOUNT_LENGTH_MAX] = {0};
    while (get_normalcount() < CONNECTION_NORMAL_MAX &&
           can_entercenter() &&
           CONNECTION_QUEUE_TURN_POINTER->getout(connectionid, name)) {
      connection::Login *loginconnection = 
        dynamic_cast<connection::Login *>(pool_->get(connectionid));
      Assert(loginconnection);
      if (0 == strcmp(loginconnection->getaccount(), name) &&
          kPlayerStatusLoginProcessTurn == loginconnection->getstatus()) {
        loginconnection->setstatus(kPlayerStatusLoginNormal);
        inc_normalcount();
        login_toclient::TurnStatus message;
        message.set_turnstatus(kLoginTurnStatusNormal);
        loginconnection->sendpacket(&message);
      } else {
        continue;
      }
      count = connection::manager::Login::getcount();
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Login::can_entercenter() const {
  __ENTER_FUNCTION
    bool result = CONNECTION_COUNTER_CENTER_POINTER->get_center_playercount() + 
                  get_normalcount() < 
                  CONNECTION_COUNTER_CENTER_POINTER->get_playercount_max();
    return result;
  __LEAVE_FUNCTION
    return false;
}
