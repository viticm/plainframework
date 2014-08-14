/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id character.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/08/13 14:51
 * @uses database for character module
 */
#ifndef DB_CHARACTER_H_
#define DB_CHARACTER_H_

#include "db/config.h"

namespace db {

namespace character {

uint8_t getnumber(const char *account); //获取角色数量

}; //namespace character

}; //namespace db

#endif //DB_CHARACTER_H_
