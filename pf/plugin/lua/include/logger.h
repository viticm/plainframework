/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id logger.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2017/01/17 11:24
 * @uses your description
*/
#include "config.h"

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
