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
#include "pf/net/connection/base.h"
#include "pf/net/packet/base.h"
#include "common/define/macros.h"
#include "character/setting.h"

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
   virtual bool isserver() const;
   virtual bool isplayer() const;
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
   int64_t getguid(int8_t index) const;
   void set_queueposition(uint16_t position);
   uint16_t get_queueposition() const;
   void set_gatewaytime(uint32_t time);
   uint32_t get_gatewaytime() const;
   void set_last_sendmessage_turntime(uint32_t time);
   uint32_t get_last_sendmessage_turntime() const;
   void set_role_baseinfo(int64_t guid, uint8_t level, int8_t index);
   bool is_guidowner(int64_t guid) const;
   uint8_t get_rolelevel(int64_t guid) const;

 private:
   char account_[ACCOUNT_LENGTH_MAX];
   character::base_t role_baselist_[DB_ROLE_MAX]; //character == role
   uint8_t rolenumber_;
   uint32_t kicktime_;
   uint32_t connecttime_;
   uint32_t gatewaytime_;
   uint32_t last_sendmessage_turntime_;
   uint32_t readykick_count_;
   uint16_t queueposition_;

};

}; //namespace connection

#endif //CONNECTION_LOGIN_H_
