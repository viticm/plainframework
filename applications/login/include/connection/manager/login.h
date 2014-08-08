/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id login.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/08/08 19:32
 * @uses connection manager login class
 */
#ifndef CONNECTION_MANAGER_LOGIN_H_
#define CONNECTION_MANAGER_LOGIN_H_

#include "connection/manager/config.h"
#include "pf/base/singleton.h"
#include "pf/base/hashmap/config.h"
#include "pf/net/manager.h"

namespace connection {

namespace manager {

class Login : public pf_base::Singleton<Login>, public pf_net::Manager {

 public:
   Login();
   ~Login();

 public:
   static Login *getsingleton_pointer();
   static Login &getsingleton();

 public:
   virtual bool processinput();
   virtual bool remove(pf_net::connection::Base *connection);

 public:
   bool init_pool();
   bool remove(const char *account, int16_t id);
   uint16_t get_normalcount() const;
   void inc_normalcount();
   void dec_normalcount();

 private:
   uint16_t normalcount_;

};

}; //namespace manager

}; //namespace connection

extern connection::manager::Login *g_connection_manager_login;

#define CONNECTION_MANAGER_LOGIN_POINTER \
  connection::manager::Login::getsingleton_pointer()

#endif //CONNECTION_MANAGER_LOGIN_H_
