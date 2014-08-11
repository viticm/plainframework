/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id turn.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/08/08 19:27
 * @uses connection counter turn class
 */
#ifndef CONNECTION_QUEUE_TURN_H_
#define CONNECTION_QUEUE_TURN_H_

#include "connection/queue/config.h"
#include "pf/base/singleton.h"
#include "pf/sys/thread.h"

namespace connection {

namespace queue {

class Turn : public pf_base::Singleton<Turn> {

 public:
   Turn();
   ~Turn();

 public
   static Turn *getsingleton_pointer();
   static Turn &getsingleton();

 public:
   bool init();
   bool addin(int16_t id, const char *name, uint16_t &queueposition);
   bool getout(int16_t &id, char *name);
   uint16_t calculate_turnnumber(uint16_t queueposition) const;
   TurnInfo &get(uint32_t queueposition);
   bool isempty() const;
   uint16_t getcount() const;
   uint16_t gethead() const;
   void erase(const char *name, int16_t id);

 private:
   TurnInfo *queue_;
   uint16_t size_;
   uint16_t head_;
   uint16_t tail_;
   pf_sys::ThreadLock lock_;

};

}; //namespace queue

}; //namespace connection

#define CONNECTION_QUEUE_TURN_POINTER \
  connection::queue::Turn::getsingleton_pointer()

extern connection::queue::Turn *g_connection_queue_turn;

#endif //CONNECTION_QUEUE_TURN_H_
