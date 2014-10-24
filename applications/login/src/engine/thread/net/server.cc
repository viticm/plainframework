#include "pf/base/log.h"
#include "pf/base/time_manager.h"
#include "pf/base/util.h"
#include "connection/manager/incoming.h"
#include "connection/manager/login.h"
#include "connection/counter/center.h"
#include "engine/thread/net/server.h"

using namespace engine::thread::net;

Server::Server() {
  isactive_ = false;
}

Server::~Server() {
  //do nothing
}

bool Server::init() {
  __ENTER_FUNCTION
    if (!connection::manager::Server::init(kConnectServerTypeNumber)) 
      return false;
    isactive_ = connection::manager::Server::init_pool();
    return isactive_;
  __LEAVE_FUNCTION
    return false;
}

void Server::run() {
  __ENTER_FUNCTION
    SLOW_LOG(NET_MODULENAME,
             "[engine.thread.net] (Server::run)"
             " thread id: %"PRIu64", server id: %d",
             pf_sys::get_current_thread_id(),
             get_current_serverid());
    while (isactive()) {
      if (!timer_.isstart()) {
        timer_.start(60000, TIME_MANAGER_POINTER->get_tickcount());
      }
      try {
        pf_base::util::sleep(1); 
        connection::manager::Server::tick();
        if (timer_.counting(TIME_MANAGER_POINTER->get_tickcount())) {
          SLOW_LOG(NET_MODULENAME,
                   "[engine.thread.net] (Server::run)"
                   " connection count map ->"
                   " incoming: %d, login: %d, login normal: %d, center: %d",
                   CONNECTION_MANAGER_INCOMING_POINTER->getcount(),
                   CONNECTION_MANAGER_LOGIN_POINTER->getcount(),
                   CONNECTION_MANAGER_LOGIN_POINTER->get_normalcount(),
                   CONNECTION_COUNTER_CENTER_POINTER->get_center_playercount());
        }
      } catch (...) {
        SaveErrorLog();
      }
    }
  __LEAVE_FUNCTION
}

void Server::stop() {
  isactive_ = false;
}

bool Server::isactive() const {
  return isactive_;
}
