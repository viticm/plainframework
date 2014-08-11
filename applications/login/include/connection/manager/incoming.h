/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id incoming.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/08/08 15:27
 * @uses connection manager incoming class
 *       cn: 客户端连接接受管理器，客户端的连接由此进入，此管理器将替换主管理器
 */
#ifndef CONNECTION_MANAGER_INCOMING_H_
#define CONNECTION_MANAGER_INCOMING_H_

#include "connection/manager/config.h"
#include "pf/base/singleton.h"
#include "pf/base/hashmap/config.h"
#include "pf/net/manager.h"

namespace connection {

namespace manager {

class Incoming : public pf_base::Singleton<Incoming>, public pf_net::Manager {

 public:
   Incoming();
   ~Incoming();

 public:
   static Incoming *getsingleton_pointer();
   static Incoming &getsingleton();

 public:
   virtual pf_net::connection::Base *accept(); //重写接受方法
   virtual bool heartbeat(uint32_t time = 0);
   virtual bool erase(pf_net::connection::Base *connection);
   virtual bool remove(pf_net::connection::Base *connection);

 protected:
   typedef hash_map<uint32_t, int32_t> ipcount_t;
   ipcount_t ipcount_;

 protected:
   bool is_toomany_clientin_sameip(uint32_t uint32host, const char *host);
   void dec_clientin_sameip(uint32_t uint32host, const char *host);

};

}; //namespace manager

}; //namespace connection

extern connection::manager::Incoming *g_connection_manager_incoming;

#define CONNECTION_MANAGER_INCOMING_POINTER \
  connection::manager::Incoming::getsingleton_pointer()

#endif //CONNECTION_MANAGER_INCOMING_H_
