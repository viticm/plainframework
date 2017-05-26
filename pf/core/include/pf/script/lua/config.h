/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id config.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2017/01/10 19:24
 * @uses The script module for lua config file.
*/
#ifndef PF_SCRIPT_LUA_CONFIG_H_
#define PF_SCRIPT_LUA_CONFIG_H_

#include "pf/script/config.h"
#include "pf/basic/type/variable.h"

#ifdef PF_OPEN_LUA

namespace pf_script {

namespace lua {

class FileBridge;
class VM;
class System;

}; //namespace lua

}; //namespace pf_script

#ifndef check_lua_argc
#define check_lua_argc(L,n) Assert(lua_gettop((L)) == (n))
#endif

#endif

#endif //PF_SCRIPT_LUA_CONFIG_H_
