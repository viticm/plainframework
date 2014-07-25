/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id outputstream.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.it@gmail.com>
 * @date 2014/06/21 12:03
 * @uses socket output stream class
 */
#ifndef PF_NET_SOCKET_OUTPUTSTREAM_H_
#define PF_NET_SOCKET_OUTPUTSTREAM_H_

#include "pf/net/packet/base.h"
#include "pf/net/socket/base.h"
#include "pf/net/socket/stream.h"

namespace pf_net {

namespace socket {

class OutputStream : public Stream {

 public:
   OutputStream(
     socket::Base* socket, 
       uint32_t bufferlength = SOCKETOUTPUT_BUFFERSIZE_DEFAULT,
       uint32_t bufferlength_max = SOCKETOUTPUT_DISCONNECT_MAXSIZE)
     : Stream(socket, bufferlength, bufferlength_max) {};
   ~OutputStream() {};

 public:
   uint32_t write(const char *buffer, uint32_t length);
   bool writepacket(const packet::Base* packet);
   int32_t flush();
 
};

}; //namespace socket

}; //namespace pf_net


#endif //PF_NET_SOCKET_OUTPUTSTREAM_H_
