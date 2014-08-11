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
#include "pf/net/manager.h"

typedef enum {
  kServerTypeCenter = 0, //中心服务器
  kServerTypeGateway, //网关服务器
  kServerTypeNumber,
} servertype_t;

namespace connection {

namespace manager {

class Server : public pf_base::Singleton<Server>, public pf_net::Manager {

 public:
   Server();
   ~Server();

 public:
   static Server *getsingleton_pointer();
   static Server &getsingleton();

 public:
   virtual bool loop();
   virtual bool heartbeat(uint32_t time = 0);

 public:
   int16_t get_current_serverid() const;
   bool syncpacket(pf_net::packet::Base *packet, 
                   uint8_t servertype, 
                   uint8_t flag = kPacketFlagNone);
   pf_net::connection::Server *get_serverconnection(uint8_t servertype);
   bool is_serverconnected(uint8_t servertype) const;
   bool is_allserver_connected() const;
   bool connectserver(uint8_t servertype);
   bool send_queue_toworld();
   void notify_totalcount_togateway();
   
 private:
   int16_t servertype_[kServerTypeNumber];
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