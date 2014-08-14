/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id server.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/08/11 16:10
 * @uses connection manager for server
 */
#ifndef CONNECTION_MANAGER_SERVER_H_
#define CONNECTION_MANAGER_SERVER_H_

#include "connection/manager/config.h"
#include "pf/base/tinytimer.h"
#include "pf/base/singleton.h"
#include "common/net/manager/server.h"

typedef enum {
  kConnectServerTypeCenter = 0,
  kConnectServerTypeGateway,
  kConnectServerTypeNumber
} connect_servertype_t; //连接的服务器类型，区别于servertype_t

namespace connection {

namespace manager {

class Server : public pf_base::Singleton<Server>, 
  public common::net::manager::Server {

 public:
   Server();
   virtual ~Server();

 public:
   static Server *getsingleton_pointer();
   static Server &getsingleton();

 public:
   //重写初始化操作，现在只留接口，以后可能会有其他操作如加密等
   virtual bool init(uint16_t connectionmax = NET_CONNECTION_MAX,
                     uint16_t listenport = 0,
                     const char *listenip = NULL);
   virtual void tick();
   virtual bool heartbeat(uint32_t time = 0);

 public:
   int16_t get_current_serverid() const;
   bool syncpacket(pf_net::packet::Base *packet, 
                   uint8_t servertype, 
                   uint8_t flag = kPacketFlagNone);
   common::net::connection::Server *get_serverconnection(uint8_t servertype);
   bool is_serverconnected(uint8_t type);
   bool is_allserver_connected();
   bool connectserver(uint8_t type);
   bool send_queue_tocenter();
   void notify_totalcount_togateway();
   bool init_pool();
   
 private:
   int16_t serverids_[kConnectServerTypeNumber]; //连接服务器ID数组，同连接ID数据一样
   pf_base::TinyTimer usercount_timer_;

};

}; //namespace manager

}; //namespace connection

#define CONNECTION_MANAGER_SERVER_POINTER \
  connection::manager::Server::getsingleton_pointer()

extern bool g_stopservice;
extern bool g_stopdb;
extern connection::manager::Server *g_connection_manager_server;

#endif //CONNECTION_MANAGER_SERVER_H_
