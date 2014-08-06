/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id setting.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/08/06 13:53
 * @uses character setting module
 */
#ifndef CHARACTER_SETTING_H_
#define CHARACTER_SETTING_H_

#include "character/config.h"

namespace character {

typedef struct new_struct {
  int64_t guid; //唯一ID
  char name[ROLE_NAME_LENGTH_MAX];
  uint8_t sex;
  uint8_t profession;
  new_struct();
  ~new_struct();
  void setguid(int64_t _guid);
  int64_t getguid();
  void setname(const char *_name);
  const char *getname();
  void setsex(uint8_t _sex);
  uint8_t getsex();
  void setprofession(uint8_t profession);
  uint8_t getprofession();
} new_t; //新建玩家数据结构

}; //namespace character

#endif //CHARACTER_SETTING_H_
