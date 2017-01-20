/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id config.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2017/01/18 19:49
 * @uses The simple app net packet config file.
*/
#ifndef PACKET_CONFIG_H_
#define PACKET_CONFIG_H_

#include "_config.h"

enum {
  kPacketIdMin = 1,
  kPacketIdSayHello = 1,
  kPacketIdHelloWorld = 2,
  kPacketIdMax = 2,
};

#endif //PACKET_CONFIG_H_
