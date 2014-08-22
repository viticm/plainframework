/**
 * PAP Engine ( https://github.com/viticm/pap )
 * $Id tab.h
 * @link https://github.com/viticm/pap for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm@126.com )
 * @license
 * @user viticm<viticm@126.com>
 * @date 2014/08/21 16:08
 * @uses tab functions for lua
*/
#ifndef COMMON_SCRIPT_LUA_TABFILE_H_
#define COMMON_SCRIPT_LUA_TABFILE_H_

#include "common/script/lua/config.h"

namespace common {

namespace script {

namespace lua {

int32_t file_opentab(lua_State *L);
int32_t file_openini(lua_State *L); //所有值和键都为string，使用需类型转换
bool register_functiontable_file();

}; //namespace lua

}; //namespace script

}; //namespace common

#endif //COMMON_SCRIPT_LUA_TAB_H_
