/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id login.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/08/13 17:05
 * @uses engine thread net login class
 */
#ifndef ENGINE_THREAD_NET_LOGIN_H_
#define ENGINE_THREAD_NET_LOGIN_H_

#include "engine/thread/net/config.h"
#include "pf/sys/thread.h"
#include "connection/manager/login.h"

namespace engine {

namespace thread {

namespace net {

class Login : public pf_sys::Thread, public connection::manager::Login {

 public:
   Login();
   ~Login();

  bool init();
  virtual void run();
  virtual void stop();
  void quit();
  bool isactive() const;

 private:
   bool isactive_;

 private:
   bool dotick(uint32_t time);
   bool move_queueplayer();
   bool can_entercenter() const;

};

}; //namespace net

}; //namespace thread

}; //namespace engine

#endif //ENGINE_THREAD_NET_LOGIN_H_
