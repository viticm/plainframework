/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id system.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/07/11 10:36
 * @uses engine system class
 */
#ifndef ENGINE_SYSTEM_H_
#define ENGINE_SYSTEM_H_

#include "engine/config.h"
#include "pf/base/singleton.h"
#include "pf/engine/kernel.h"
#include "engine/thread/net/incoming.h"
#include "engine/thread/net/login.h"
#include "engine/thread/net/server.h"


namespace engine {

class System : public pf_engine::Kernel, public pf_base::Singleton<System> {

 public:
   System();
   ~System();

 public:
   static System *getsingleton_pointer();
   static System &getsingleton(); 

 public:
   bool init();

 public:
   pf_db::Manager *get_dbmanager();
   pf_net::Manager *get_netmanager();
   void set_netmanager(pf_net::Manager *netmanager);

 protected:
   virtual bool init_net();
   virtual bool init_net_connectionpool();
   virtual void run_net();
   virtual void stop_net();

 private:
   bool init_net_incoming(); //初始化网络接收管理器
   bool init_net_login(); //初始化网络登陆管理器
   bool init_net_server(); //初始化网络服务器连接管理器
   void run_net_incoming();
   void run_net_login();
   void run_net_server();
   void stop_net_incoming();
   void stop_net_login();
   void stop_net_server();
   bool init_setting();

 private:
   thread::net::Incoming *incoming_netmanager_; //这个指针会赋给基础的网络管理器
   thread::net::Login *login_netmanager_;
   thread::net::Server *server_netmanager_;

};

}; //namespace engine

#define ENGINE_SYSTEM_POINTER engine::System::getsingleton_pointer()

extern engine::System *g_engine_system;

#endif //ENGINE_SYSTEM_H_
