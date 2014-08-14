/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id center.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/08/08 19:23
 * @uses connection queue center class
 */
#ifndef CONNECTION_QUEUE_CENTER_H_
#define CONNECTION_QUEUE_CENTER_H_

#include "connection/queue/config.h"
#include "pf/base/singleton.h"
#include "pf/sys/thread.h"

namespace connection {

namespace queue {

class Center : public pf_base::Singleton<Center> {

 public:
   Center();
   ~Center();

 public:
   static Center *getsingleton_pointer();
   static Center &getsingleton();

 public:
   bool init();
   bool addin(int16_t id, 
              const char *name, 
              int64_t guid, 
              uint16_t &queueposition);
   bool getout(int16_t &id, char *name);
   bool findhead(uint16_t &queueposition);
   centerinfo_t &get(uint16_t queueposition);

 public:
   bool isempty() const;
   uint16_t getcount() const;
   uint16_t gethead() const;

 private:
   centerinfo_t *queue_;
   uint16_t size_;
   uint16_t head_;
   uint16_t tail_;
   pf_sys::ThreadLock lock_;

};

}; //namespace queue

}; //namespace connection

#define CONNECTION_QUEUE_CENTER_POINTER \
  connection::queue::Center::getsingleton_pointer()

extern connection::queue::Center *g_connection_queue_center;

#endif //CONNECTION_QUEUE_CENTER_H_
