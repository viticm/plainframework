/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id config.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/08/06 10:24
 * @uses your description
 */
#ifndef CONNECTION_CONFIG_H_
#define CONNECTION_CONFIG_H_

#include "application/config.h"

namespace connection {

typedef turninfo_struct {
  int16_t id;
  char name[ACCOUNT_LENGTH_MAX];
  bool isused;
  uint16_t queueposition;
  turninfo_struct() {
    id = ID_INVALID;
    memset(name, 0, sizeof(name));
    isused = false;
    queueposition = 0;
  }
} turninfo_t; //排队连接信息结构

typedef centerinfo_struct {
  int16_t id;
  char name[ACCOUNT_LENGTH_MAX];
  bool isused;
  int64_t guid;
  int16_t sceneid;
  uint16_t queueposition;
  centerinfo_struct {
    id = ID_INVALID;
    memset(name, 0, sizeof(name));
    isused = false;
    guid = ID_INVALID;
    sceneid = ID_INVALID;
    queueposition = 0;
  }
} centerinfo_t; //中心连接信息结构

}; //namespace connection

#endif //CONNECTION_CONFIG_H_
