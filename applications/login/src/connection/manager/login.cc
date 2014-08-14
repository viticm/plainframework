#include "pf/base/log.h"
#include "pf/base/util.h"
#include "common/setting.h"
#include "common/define/enum.h"
#include "common/net/packet/login_togateway/playerleave.h"
#include "common/define/net/packet/id/gatewaylogin.h"
#include "engine/system.h"
#include "pf/net/packet/factorymanager.h"
#include "connection/login.h"
#include "connection/manager/server.h"
#include "connection/manager/login.h"

#define READY_HEARTBEAT_MAX 100

connection::manager::Login *g_connection_manager_login = NULL;

template <>
connection::manager::Login 
  *pf_base::Singleton<connection::manager::Login>::singleton_ = NULL;

namespace connection {

namespace manager {

Login::Login() {
  set_poll_maxcount(SETTING_POINTER->login_info_.net_connectionmax);
  is_servermode_ = false;
  normalcount_ = 0;
}

Login::~Login() {
  //do nothing
}

Login *Login::getsingleton_pointer() {
  return singleton_;
}

Login &Login::getsingleton() {
  Assert(singleton_);
  return *singleton_;
}

#if __LINUX__ && defined(_PF_NET_EPOLL) /* { */ 
bool Login::processinput() {
  __ENTER_FUNCTION
    using namespace pf_base;
    uint16_t i; 
    for (i = 0; i < polldata_.result_eventcount; ++i) { 
      int32_t socketid = static_cast<int32_t>(
          util::get_highsection(polldata_.events[i].data.u64));
      int16_t connectionid = static_cast<int16_t>( 
          util::get_lowsection(polldata_.events[i].data.u64));
      if (socketid <= SOCKET_INVALID || ID_INVALID == connectionid) continue;
      connection::Login *loginconnection = 
        dynamic_cast<connection::Login *>(get(connectionid));
      Assert(loginconnection);
      if (kPlayerStatusLoginServerReady == loginconnection->getstatus()) {
        loginconnection->set_readykick_count(
            loginconnection->get_readykick_count() + 1);
        if (loginconnection->get_readykick_count() > READY_HEARTBEAT_MAX)
          remove(connection);
      }
      if (loginconnection->getstatus() != kPlayerStatusLoginNormal ||
          loginconnection->getstatus() != kPlayerStatusLoginServerReady) {
        continue;
      }
      int32_t socketid = loginconnection->getsocket()->getid();
      if (SOCKET_INVALID == socketid) {
        SLOW_ERRORLOG(NET_MODULENAME,
                      "[connection.manager] (Login::processinput)"
                      " error! socket id is invalid ");
        continue;
      }
      if (!loginconnection->getsocket()->iserror()) {
        remove(loginconnection);
      } else {
        try {
          if (!loginconnection->processinput()) remove(loginconnection);
        } catch (...) {
          remove(loginconnection);
        }
      }
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}
#elif __WINDOWS__ && defined(_PF_NET_IOCP) /* }{ */ 
#error "net iocp not completed"
bool Login::processinput() {
  __ENTER_FUNCTION
  
  __LEAVE_FUNCTION
    return false;
}
#else /* }{ */
bool Login::processinput() {
  __ENTER_FUNCTION
    if (SOCKET_INVALID == minfd_ && SOCKET_INVALID == maxfd_) return true;
    uint16_t i;
    uint16_t connectioncount = Base::getcount();
    for (i = 0; i < connectioncount; ++i) {
      if (ID_INVALID == connection_idset_[i]) continue;
      connection::Login *loginconnection = 
        dynamic_cast<connection::Login *>(get(connection_idset_[i]));
      Assert(loginconnection);
      if (kPlayerStatusLoginServerReady == loginconnection->getstatus()) {
        loginconnection->set_readykick_count(
            loginconnection->get_readykick_count());
        if (loginconnection->get_readykick_count() > READY_HEARTBEAT_MAX)
          remove(loginconnection);
      }
      if (loginconnection->getstatus() != kPlayerStatusLoginNormal ||
          loginconnection->getstatus() != kPlayerStatusLoginServerReady) {
        continue;
      }
      int32_t socketid = loginconnection->getsocket()->getid();
      if (SOCKET_INVALID == socketid) {
        SLOW_ERRORLOG(NET_MODULENAME,
                      "[connection.manager] (Login::processinput)"
                      " error! socket id is invalid ");
        continue;
      }
      if (FD_ISSET(socketid, &readfds_[kSelectUse])) {
        if (!loginconnection->getsocket()->iserror()) {
          remove(loginconnection);
        } else {
          try {
            if (!loginconnection->processinput()) remove(loginconnection);
          } catch (...) {
            remove(loginconnection);
          }
        }
      }
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}
#endif /* } */

bool Login::init_pool() {
  __ENTER_FUNCTION
    pf_net::connection::Pool *connectionpool = 
      ENGINE_SYSTEM_POINTER->get_netmanager()->getpool();
    Assert(connectionpool); //需要引擎核心先初始化
    pool_ = connectionpool;
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Login::remove(pf_net::connection::Base *connection) {
  __ENTER_FUNCTION
    using namespace common::net::packet::login_togateway;
    using namespace common::define::net::packet::id::login_togateway;
    connection::Login *loginconnection = 
      dynamic_cast<connection::Login *>(connection);
    Assert(loginconnection);
    bool result = false;
    if (kPlayerStatusLoginNormal == loginconnection->getstatus()) {
      PlayerLeave *message = dynamic_cast<PlayerLeave *>(
          NET_PACKET_FACTORYMANAGER_POINTER->createpacket(kPlayerLeave));
      if (message) {
        message->setaccount(loginconnection->getaccount());
        message->setguid(ID_INVALID);
        message->set_centerid(SETTING_POINTER->center_info_.id);
        CONNECTION_MANAGER_SERVER_POINTER->syncpacket(message, 
                                                      kServerTypeGateway);
      }
    }
    if (kPlayerStatusLoginNormal == loginconnection->getstatus() ||
        kPlayerStatusLoginServerReady == loginconnection->getstatus()) {
      dec_normalcount();
    }
    SLOW_LOG(NET_MODULENAME,
             "[connection.manager] (Login::remove)"
             " account: %s ... success",
             loginconnection->getaccount());
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Login::remove(const char *account, int16_t id) {
  __ENTER_FUNCTION
    Assert(account);
    uint16_t count = getcount();
    uint16_t i;
    for (i = 0; i < count; ++i) {
      connection::Login *loginconnection = 
        dynamic_cast<connection::Login *>(get(connection_idset_[i]));
      if (loginconnection) {
        if (kPlayerStatusLoginNormal == loginconnection->getstatus() &&
            loginconnection->getid() != id && //自身连接不删除
            0 == strcmp(loginconnection->getaccount(), account)) {
          remove(loginconnection);
          break;
        }
      }
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

} //namespace manager

} //namespace connection
