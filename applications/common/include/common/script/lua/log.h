/**
 * PAP Engine ( https://github.com/viticm/pap )
 * $Id log.h
 * @link https://github.com/viticm/pap for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm@126.com )
 * @license
 * @user viticm<viticm@126.com>
 * @date 2014/08/21 11:26
 * @uses log functions for lua
*/
#ifndef COMMON_SCRIPT_LUA_LOG_H_
#define COMMON_SCRIPT_LUA_LOG_H_

#include "common/script/lua/config.h"
#include <lua.hpp>

namespace common {

namespace script {

namespace lua {

int32_t slow_log(lua_State *L);
int32_t slow_errorlog(lua_State *L);
int32_t slow_warninglog(lua_State *L);
int32_t slow_debuglog(lua_State *L);
int32_t slow_writelog(lua_State *L);
int32_t fast_log(lua_State *L);
int32_t fast_errorlog(lua_State *L);
int32_t fast_warninglog(lua_State *L);
int32_t fast_debuglog(lua_State *L);
int32_t fast_writelog(lua_State *L);
bool register_functiontable_log(); //注册全局的方法表

}; //namespace lua

}; //namespace script

}; //namespace common

#endif //COMMON_SCRIPT_LUA_LOG_H_
