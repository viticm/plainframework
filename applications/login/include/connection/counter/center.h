/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id center.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/08/08 19:23
 * @uses connection counter center class
 */
#ifndef CONNECTION_COUNTER_CENTER_H_
#define CONNECTION_COUNTER_CENTER_H_

#include "connection/counter/config.h"
#include "pf/base/singleton.h"

namespace connection {

namespace counter {

class Center : public pf_base::Singleton<Center> {

 public:
   Center();
   ~Center();

 public:
   static Center *getsingleton_pointer();
   static Center &getsingleton();

 public:
   uint16_t get_require_playercount() const;
   void set_center_playercount(uint16_t count);
   uint16_t get_center_playercount() const;
   void set_playercount_max(uint16_t count);
   uint16_t get_playercount_max() const;

 private:
   uint16_t center_playercount_;
   uint16_t playercount_max_;

};

}; //namespace counter

}; //namespace connection

#define CONNECTION_COUNTER_CENTER_POINTER \
  connection::counter::Center::getsingleton_pointer()

extern connection::counter::Center *g_connection_counter_center;

#endif //CONNECTION_COUNTER_CENTER_H_
