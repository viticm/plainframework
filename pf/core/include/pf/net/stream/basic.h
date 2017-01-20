/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id stream.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.it@gmail.com>
 * @date 2014/06/21 12:32
 * @uses net socket stream class
 *       docs link: http://www.cnblogs.com/lianyue/p/4265833.html
 */
#ifndef PF_NET_STREAM_STREAM_H_
#define PF_NET_STREAM_STREAM_H_

#include "pf/net/socket/config.h"
#include "pf/basic/string.h"
#include "pf/net/stream/compressor.h"

namespace pf_net {

namespace stream {

class PF_API Basic {

 public:
   Basic(
       socket::Basic *socket, 
       uint32_t bufferlength = NETINPUT_BUFFERSIZE_DEFAULT, 
       uint32_t bufferlength_max = NETINPUT_DISCONNECT_MAXSIZE); 
   virtual ~Basic();

 public:
   void init();
   bool resize(int32_t size);
   uint32_t reallength() const;
   void clear();
   socket::Basic *socket() { return socket_; };
   Compressor *getcompressor() { return &compressor_; };
   Encryptor *getencryptor() { return &encryptor_; };

 public:
   bool encrypt_isenable() const { return encrypt_isenable_; };
   void encryptenable(bool enable) { 
     encrypt_isenable_ = enable;
   };
   void compressenable(bool enable) {
     compressor_.getassistant()->enable(enable);
   };
   void encrypt_setkey(const char * key){
     encryptor_.setkey(key);
   };
   bool isinit() const { return isinit_; };
   void set_isinit(bool isinit) {  isinit_ = isinit; };

 protected:
   socket::Basic *socket_;
   Encryptor encryptor_;
   socket::streamdata_t streamdata_;
   Compressor compressor_;
   bool encrypt_isenable_;
   uint64_t send_bytes_;
   uint64_t receive_bytes_;
   bool isinit_;

};

}; //namespace stream

}; //namespace pf_net

#endif //PF_NET_STREAM_STREAM_H_
