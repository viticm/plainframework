#ifndef PACKET_SAYHELLO_H_
#define PACKET_SAYHELLO_H_

#include "packet/config.h"

class SayHello : public pf_net::packet::Interface {


 public:
   SayHello() : str_{0} {}
   virtual ~SayHello() {}

 public:
   virtual bool read(pf_net::stream::Input &);
   virtual bool write(pf_net::stream::Output &);
   virtual uint32_t execute(pf_net::connection::Basic *connection);
   uint16_t get_id() const { return kPacketIdSayHello; };
   virtual uint32_t size() const { 
     return sizeof(uint32_t) + static_cast<uint32_t>(strlen(str_)); 
   };
   void set_str(const std::string &str) {
     pf_basic::string::safecopy(str_, str.c_str(), sizeof(str_));
   }
   const char *get_str() {
     return str_;
   }

 private:
   char str_[128];

};

class SayHelloFactory : public pf_net::packet::Factory {

 public:
   SayHelloFactory() {}
   virtual ~SayHelloFactory() {}

 public:
   virtual pf_net::packet::Interface *packet_create() {
     return new SayHello();
   }
   uint16_t packet_id() const {
     return kPacketIdSayHello;
   }
   virtual uint32_t packet_max_size() const {
     return 128;
   };

};

class SayHelloHandler { 

 public:
   static uint32_t execute(
       SayHello *packet, pf_net::connection::Basic *connection);
};

#endif //PACKET_SAYHELLO_H_
