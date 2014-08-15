#include "pf/base/log.h"
#include "common/setting.h"
#include "common/net/packet/login_togateway/playeronline.h"
#include "engine/system.h"
#include "connection/counter/center.h"
#include "connection/login.h"
#include "connection/queue/center.h"
#include "connection/manager/server.h"

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
    for (i = 0; i < kConnectServerTypeNumber; ++i) {
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
    getpool()->init(kConnectServerTypeNumber); //连接的数量
    uint16_t i;
    for (i = 0; i < pool_->get_maxcount(); ++i) {
      common::net::connection::Server *connection = 
        new common::net::connection::Server();
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
    for (i = 0; i < kConnectServerTypeNumber; ++i) {
      if (!is_serverconnected(i)) {
        connectserver(i);
      }
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

int16_t Server::get_current_serverid() const {
  __ENTER_FUNCTION
    if (!SETTING_POINTER) return ID_INVALID;
    int16_t serverid = SETTING_POINTER->login_info_.id;
    return serverid;
  __LEAVE_FUNCTION
    return ID_INVALID;
}

bool Server::syncpacket(pf_net::packet::Base *packet,
                        uint8_t servertype,
                        uint8_t flag) {
  __ENTER_FUNCTION
    USE_PARAM(flag); //now not useit
    common::net::connection::Server *serverconnection = 
      get_serverconnection(servertype);
    bool result = serverconnection->sendpacket(packet);
    return result;
  __LEAVE_FUNCTION
    return false;
}

common::net::connection::Server *Server::get_serverconnection(
    uint8_t servertype) {
  __ENTER_FUNCTION
    Assert(servertype >= 0 && servertype <= kConnectServerTypeNumber);
    int16_t serverid = serverids_[servertype];
    common::net::connection::Server *serverconnction = 
      dynamic_cast<common::net::connection::Server *>(get(serverid));
    return serverconnction;
  __LEAVE_FUNCTION
    return NULL;
}

bool Server::is_serverconnected(uint8_t type) {
  __ENTER_FUNCTION
    common::net::connection::Server *serverconnection = get_serverconnection(type);
    if (NULL == serverconnection) return false;
    bool result = serverconnection->isvalid();
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Server::is_allserver_connected() {
  __ENTER_FUNCTION
    uint8_t i;
    for (i = 0; i < kConnectServerTypeNumber; ++i) {
      if (!is_serverconnected(i)) return false;
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Server::connectserver(uint8_t type) {
  __ENTER_FUNCTION
    bool result = false;
    switch (type) {
      case kConnectServerTypeCenter: {
        result = connect_toserver(
            SETTING_POINTER->server_info_.center_data.ip,
            SETTING_POINTER->server_info_.center_data.port,
            get_current_serverid(),
            serverids_[type]);
        break;
      }
      case kConnectServerTypeGateway: {
        result = connect_toserver(
            SETTING_POINTER->gateway_info_.ip_,
            SETTING_POINTER->gateway_info_.port_,
            get_current_serverid(),
            serverids_[type]);
        break;
      }
      default:
        break;
    }
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Server::send_queue_tocenter() {
  __ENTER_FUNCTION
    uint16_t queueposition;
    while (CONNECTION_QUEUE_CENTER_POINTER->findhead(queueposition)) {
      centerinfo_t &centerinfo = 
        CONNECTION_QUEUE_CENTER_POINTER->get(queueposition);  
      pf_net::connection::Pool *connectionpool = 
        ENGINE_SYSTEM_POINTER->get_netmanager()->getpool();
      Login *loginconnection = 
        dynamic_cast<Login *>(connectionpool->get(centerinfo.id));
      if (NULL == loginconnection) {
        SLOW_ERRORLOG(NET_MODULENAME,
                      "[connection.manager] Server::send_queue_tocenter()"
                      " NULL == loginconnection."
                      " queueposition: %d, connection id: %d, account: %s",
                      queueposition,
                      centerinfo.id,
                      centerinfo.name);
        continue;
      }

    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

void Server::notify_totalcount_togateway() {
  __ENTER_FUNCTION
    using namespace common::net::packet::login_togateway;
    uint32_t now = TIME_MANAGER_POINTER->get_current_time();
    if (!onlinetimer_.isstart()) {
      onlinetimer_.start(1000 * 300, now);
    }
    if (onlinetimer_.counting(now)) {
      PlayerOnline message;
      message.set_centerid(SETTING_POINTER->center_info_.id);
      message.setonline(CONNECTION_COUNTER_CENTER_POINTER->get_center_playercount());
      common::net::connection::Server *serverconnection = 
        get_serverconnection(kConnectServerTypeGateway);
      if (serverconnection) serverconnection->sendpacket(&message);
    }
  __LEAVE_FUNCTION
}

} //namespace manager

} //namespace connection
