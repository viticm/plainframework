/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id server.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/08/14 14:19
 * @uses engine thread for server net class
 */
#ifndef ENGINE_THREAD_NET_SERVER_H_
#define ENGINE_THREAD_NET_SERVER_H_

#include "engine/thread/net/config.h"
#include "pf/sys/thread.h"
#include "pf/base/tinytimer.h"
#include "connection/manager/server.h"

namespace engine {

namespace thread {

namespace net {

class Server : public pf_sys::Thread, public connection::manager::Server {

 public:
   Server();
   ~Server();

 public:
   bool init();
   virtual void run();
   virtual void stop();
   bool isactive() const;

 private:
   bool isactive_;
   pf_base::TinyTimer timer_;

};

}; //namespace net

}; //namespace thread

}; //namespace engine

#endif //ENGINE_THREAD_NET_SERVER_H_
