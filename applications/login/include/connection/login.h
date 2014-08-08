/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id login.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/08/06 11:05
 * @uses connection of login
 */
#ifndef CONNECTION_LOGIN_H_
#define CONNECTION_LOGIN_H_

#include "connection/config.h"
#include "common/define/macros.h"
#include "pf/net/connection/base.h"
#include "pf/net/packet/base.h"

#define CONNECTION_KICKTIME_MAX 9000000
#define CONNECTION_TURNMESSAGE_TIME_MAX 5000

namespace connection {

class Login : public pf_net::connection::Base {

 public:
   Login();
   virtual ~Login();

 public:
   virtual bool init();
   virtual bool heartbeat(uint32_t time = 0);
   virtual void clear();
   virtual bool isplayer() const;
   virtual bool isserver() const;
   virtual void resetkick();

 public:
   bool flushdirectly(pf_net::packet::Base *packet);
   bool senddirectly(pf_net::packet::Base *packet);

 public:
   uint8_t get_characternumber() const;
   void set_characternumber(uint8_t number);
   const char *getaccount();
   void setaccount(const char *account);
   uint32_t get_connecttime() const;
   void set_connecttime(uint32_t time);
   uint32_t get_readykick_count() const;
   void set_readykick_count(uint32_t count);

 private:
   char account_[ACCOUNT_LENGTH_MAX];
   uint8_t characternumber_;
   uint32_t kicktime_;
   uint32_t connecttime_;
   uint32_t gatewaytime_;
   uint32_t last_sendmessage_turntime_;
   uint32_t readykick_count_;

};

}; //namespace connection

#endif //CONNECTION_LOGIN_H_
