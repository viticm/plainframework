#include "connection/manager/server.h"

#define CONNECTION_LOGIN_SERVER_POOLSIZE_MAX 3
bool g_stopservice = false;
connection::manager::Server *g_connection_manager_server = NULL;

namespace connection {

namespace manager {

Server::Server() {
  __ENTER_FUNCTION
    is_servermode_ = false;
    uint16_t i;
    for (i = 0; i < kServerTypeNumber; ++i) {
      servertype_[i] = ID_INVALID;
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

} //namespace manager

} //namespace connection
