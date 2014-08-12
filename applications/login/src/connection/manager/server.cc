#include "common/setting.h"
#include "connection/manager/server.h"

#define CONNECTION_LOGIN_SERVER_POOLSIZE_MAX 3

bool g_stopservice = false;
connection::manager::Server *g_connection_manager_server = NULL;

template <>
connection::manager::Server 
  *pf_base::Singleton<connection::manager::Server>::singleton_ = NULL;

namespace connection {

namespace manager {

Server::Server() {
  __ENTER_FUNCTION
    is_servermode_ = false;
    uint16_t i;
    for (i = 0; i < kServerTypeNumber; ++i) {
      serverids_[i] = ID_INVALID;
    }
    set_poll_maxcount(32);
  __LEAVE_FUNCTION
}

Server::~Server() {
  //do nothing
}

bool Server::init(uint16_t connectionmax,
                  uint16_t listenport,
                  const char *listenip) {
  __ENTER_FUNCTION
    bool result = false;
    result = Manager::init(connectionmax, listenport, listenip); 
    return result;
  __LEAVE_FUNCTION
    return false;
}

Server *Server::getsingleton_pointer() {
  return singleton_;
}

Server &Server::getsingleton() {
  Assert(singleton_);
  return *singleton_;
}

bool Server::init_pool() {
  __ENTER_FUNCTION
    //getpool 如果未分配池内存会自动分配
    getpool()->init(CONNECTION_LOGIN_SERVER_POOLSIZE_MAX);
    uint16_t i;
    for (i = 0; i < pool_->get_maxcount(); ++i) {
      pf_net::connection::Server *connection = new pf_net::connection::Server();
      getpool()->init_data(i, connection);
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

void Server::tick() {
  __ENTER_FUNCTION
    Manager::tick();
    try {
      send_queue_tocenter(); //循环处理中多加一项，发送排队信息到中心服务器
    } catch(...) {

    }
  __LEAVE_FUNCTION
}

bool Server::heartbeat(uint32_t time) {
  __ENTER_FUNCTION
    if (!Manager::heartbeat(time)) return false;
    notify_totalcount_togateway(); //同步连接（玩家）数量 
    static bool allserver_connected = false;
    if (is_allserver_connected()) {
      if (false == allserver_connected) { //第一次正常开启写入服务器日志
        SLOW_LOG(NET_MODULENAME, 
                 "[connection.manager] (Server::heartbeat)"
                 " first all server is connented");
        allserver_connected = true;
      }
      return true;
    }
    uint8_t i;
    for (i = 0; i < kServerTypeNumber; ++i) {
      if (!is_serverconnected(i)) {
        conncetserver(i);
      }
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

int16_t Server::get_current_serverid() const {
  if (!SETTING_POINTER) return -1;
  return SETTING_POINTER->login_info_.id;
}

bool Server::syncpacket(pf_net::packet::Base *packet,
                        uint8_t servertype,
                        uint8_t flag) {
  __ENTER_FUNCTION
    int16_t serverid = serverids_[servertype];
    
  __LEAVE_FUNCTION
    return false;
}

} //namespace manager

} //namespace connection
