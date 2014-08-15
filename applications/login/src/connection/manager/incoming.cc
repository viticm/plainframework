#include "pf/base/time_manager.h"
#include "pf/base/log.h"
#include "common/define/enum.h"
#include "common/setting.h"
#include "connection/login.h"
#include "engine/system.h"
#include "connection/manager/incoming.h"

connection::manager::Incoming *g_connection_manager_incoming = NULL;

#define LOGIN_PLAYER_AUTH_TIME_MAX 25000

template <>
connection::manager::Incoming 
  *pf_base::Singleton<connection::manager::Incoming>::singleton_ = NULL;

namespace connection {

namespace manager {

Incoming::Incoming() {
  set_poll_maxcount(SETTING_POINTER->login_info_.net_connectionmax);
}

Incoming::~Incoming() {
  //do nothing
}

Incoming *Incoming::getsingleton_pointer() {
  return singleton_;
}

Incoming &Incoming::getsingleton() {
  Assert(singleton_);
  return *singleton_;
}

pf_net::connection::Base *Incoming::accept() {
  __ENTER_FUNCTION
    pf_net::connection::Base *connection = Manager::accept();
    connection::Login *loginconnection = 
      dynamic_cast<connection::Login *>(connection); 
    if (loginconnection) {
      uint32_t currenttime = TIME_MANAGER_POINTER->get_current_time();
      loginconnection->set_connecttime(currenttime);
      loginconnection->setstatus(kPlayerStatusLoginConnect);
      uint32_t uint32host = 
        static_cast<uint32_t>(loginconnection->getsocket()->getuint64host());
      const char *host = loginconnection->getsocket()->host_;
      if (is_toomany_clientin_sameip(uint32host, host)) {
        remove(connection);
      }
    }
    return connection;
  __LEAVE_FUNCTION
    return NULL;
}

bool Incoming::heartbeat(uint32_t time) {
  __ENTER_FUNCTION
    bool result = false;
    if (0 == time) time = TIME_MANAGER_POINTER->get_current_time();
    uint16_t count = getcount();
    DEBUGPRINTF("count: %d", count);
    uint16_t i;
    for (i = 0; i < count; ++i) {
      if (ID_INVALID == connection_idset_[i]) continue;
      pf_net::connection::Base *connection = get(connection_idset_[i]);
      if (NULL == connection) {
        Assert(false);
        return false;
      }
      result = connection->heartbeat(time);
      if (!result) {
        remove(connection);
      } else {
        connection::Login *loginconnection = 
          dynamic_cast<connection::Login *>(connection);
        Assert(loginconnection);
        if (kPlayerStatusLoginConnect == loginconnection->getstatus()) {
          if (time - loginconnection->get_connecttime() > 
              LOGIN_PLAYER_AUTH_TIME_MAX) {
            remove(connection);
          }
        }
      }
    }
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Incoming::erase(pf_net::connection::Base *connection) {
  __ENTER_FUNCTION
    bool result = false;
    uint32_t uint32host = 
      static_cast<uint32_t>(connection->getsocket()->getuint64host());
    const char *host = connection->getsocket()->host_;
    dec_clientin_sameip(uint32host, host);
    result = Base::erase(connection);
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Incoming::remove(pf_net::connection::Base *connection) {
  __ENTER_FUNCTION
    bool result = false;
    uint32_t uint32host = 
      static_cast<uint32_t>(connection->getsocket()->getuint64host());
    const char *host = connection->getsocket()->host_;
    dec_clientin_sameip(uint32host, host);
    result = Base::remove(connection);
    return result;
  __LEAVE_FUNCTION
    return false;
}

void Incoming::dec_clientin_sameip(uint32_t uint32host, const char *host) {
  __ENTER_FUNCTION
    Assert(host);
    ipcount_t::iterator iterator = ipcount_.find(uint32host);
    if (iterator != ipcount_.end()) {
      --(iterator->second);
      if (iterator->second < 0) {
        SLOW_ERRORLOG(APPLICATION_NAME, 
                      "[connection.manager] (Incoming::dec_clientin_sameip)"
                      " ip: %s, count shound not < 0",
                      host);
      }
      if (0 == iterator->second) {
        ipcount_.erase(iterator);
        SLOW_ERRORLOG(APPLICATION_NAME, 
                      "[connection.manager] (Incoming::dec_clientin_sameip)"
                      " ip: %s, count: %d erase",
                      host,
                      iterator->second);

      } else {
         SLOW_ERRORLOG(APPLICATION_NAME, 
                      "[connection.manager] (Incoming::dec_clientin_sameip)"
                      " ip: %s, count: %d",
                      host,
                      iterator->second);
      }
    }
  __LEAVE_FUNCTION
}

bool Incoming::is_toomany_clientin_sameip(uint32_t uint32host, 
                                          const char *host) {
  __ENTER_FUNCTION
    Assert(host);
    ipcount_t::iterator iterator = ipcount_.find(uint32host);
    if (iterator != ipcount_.end()) {
      if (iterator->second > 
          SETTING_POINTER->login_info_.clientin_sameip_max) {
        ++(iterator->second);
        SLOW_WARNINGLOG(APPLICATION_NAME,
                       "[connection.manager]"
                       " (Incoming::is_toomany_clientin_sameip)"
                       " clients(%d) in same ip: %s",
                       iterator->second,
                       host);
        return false;
      } else {
        ++(iterator->second);
        SLOW_LOG(APPLICATION_NAME,
                 "[connection.manager]"
                 " (Incoming::is_toomany_clientin_sameip)"
                 " ++clients(%d) in same ip: %s",
                 iterator->second,
                 host);
      }
    } else {
      ipcount_.insert(ipcount_t::value_type(uint32host, 1));
      SLOW_LOG(APPLICATION_NAME,
               "[connection.manager]"
               " (Incoming::is_toomany_clientin_sameip)"
               " first count ip: %s",
               host);
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

} //namespace manager

} //namespace connection
