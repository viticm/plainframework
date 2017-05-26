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
#ifndef PF_PLUGIN_LUA_LOG_H_
#define PF_PLUGIN_LUA_LOG_H_

#include "config.h"

PF_PLUGIN_API int32_t slow_log(lua_State *L);
PF_PLUGIN_API int32_t slow_errorlog(lua_State *L);
PF_PLUGIN_API int32_t slow_warninglog(lua_State *L);
PF_PLUGIN_API int32_t slow_debuglog(lua_State *L);
PF_PLUGIN_API int32_t slow_writelog(lua_State *L);
PF_PLUGIN_API int32_t fast_log(lua_State *L);
PF_PLUGIN_API int32_t fast_errorlog(lua_State *L);
PF_PLUGIN_API int32_t fast_warninglog(lua_State *L);
PF_PLUGIN_API int32_t fast_debuglog(lua_State *L);
PF_PLUGIN_API int32_t fast_writelog(lua_State *L);

#endif //PF_PLUGIN_LUA_LOG_H_
