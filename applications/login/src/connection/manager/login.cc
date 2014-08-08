#include "pf/base/log.h"
#include "pf/base/util.h"
#include "common/setting.h"
#include "common/define/enum.h"
#include "connection/login.h"
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
      if (FD_ISSET(socketid, readfds_[kSelectUse])) {
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

} //namespace manager

} //namespace connection
