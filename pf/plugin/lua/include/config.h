/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id config.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2017/01/17 11:24
 * @uses The lua lib of PF.
*/
#ifndef PF_PLUGIN_LUA_CONFIG_H_
#define PF_PLUGIN_LUA_CONFIG_H_

#include <lua.hpp>
#include "pf/basic/config.h"
#include "pf/basic/logger.h"

#define SCRIPT_LUA_CHECKARGC(L, argc) { \
  Assert(L); \
  int32_t _argc = lua_gettop(L); \
  if (_argc != argc) { \
    SLOW_ERRORLOG("script", \
                  "[script.lua] file: %s, line: %d, argc: %d, expect: %d", \
                  __FILE__, \
                  __LINE__, \
                  _argc, \
                  argc);\
    return 0; \
  } \
}

#endif //PF_PLUGIN_LUA_CONFIG_H_
