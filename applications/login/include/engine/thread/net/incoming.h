/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id incoming.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/08/12 21:50
 * @uses engine thread connection manager for incoming class
 */
#ifndef ENGINE_THREAD_NET_INCOMING_H_
#define ENGINE_THREAD_NET_INCOMING_H_

#include "engine/thread/net/config.h"
#include "pf/sys/thread.h"
#include "connection/manager/incoming.h"

namespace engine {

namespace thread {

namespace net {

class Incoming : public pf_sys::Thread, public connection::manager::Incoming {

 public:
   Incoming();
   ~Incoming();

 public:
   bool init(uint16_t connectionmax = NET_CONNECTION_MAX,
             uint16_t listenport = 0,
             const char *listenip = NULL);
   virtual void run();
   virtual void stop();
   void quit();
   bool isactive() const;

 private:
   bool isactive_;

};

}; //namespace net

}; //namespace thread

}; //namespace engine

#endif //ENGINE_THREAD_CONNECTION_MANAGER_INCOMING_H_
