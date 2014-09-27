/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id script_execute.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/09/26 12:40
 * @uses 脚本查询
 */
#ifndef COMMON_NET_PACKET_SERVERCENTER_SCRIPT_EXECUTE_H_
#define COMMON_NET_PACKET_SERVERCENTER_SCRIPT_EXECUTE_H_

#include "common/net/packet/servercenter/config.h"
#include "pf/net/connection/base.h"
#include "pf/net/packet/base.h"
#include "pf/net/packet/factory.h"
#include "common/define/macros.h"

namespace common {

namespace net {

namespace packet {

namespace servercenter {

class ScriptExecute : public pf_net::packet::Base {

 public:
   ScriptExecute();
   virtual ~ScriptExecute() {};

 public:  
   virtual bool read(pf_net::socket::InputStream &inputstream);
   virtual bool write(pf_net::socket::OutputStream &outputstream) const;
   virtual uint32_t execute(pf_net::connection::Base *connection);
   virtual uint16_t getid() const;
   virtual uint32_t getsize() const;
   
 public: 
   const char *get_function();
   void set_function(const char *function);

 private:
   char function_[2048];

};

class ScriptExecuteFactory : public pf_net::packet::Factory {

 public:
   pf_net::packet::Base *createpacket();
   uint16_t get_packetid() const;
   uint32_t get_packet_maxsize() const;

};

class ScriptExecuteHandler {

 public:
   static uint32_t execute(ScriptExecute *packet, 
                           pf_net::connection::Base *connection);

};

}; //namespace servercenter

}; //namespace packet

}; //namespace net

}; //namespace common

#endif //COMMON_NET_PACKET_SERVERCENTER_SCRIPT_EXECUTE_H_
