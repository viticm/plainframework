/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id cache.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2017/04/17 14:05
 * @uses The pf cache module for lua.
*/
#ifndef PF_PLUGIN_LUA_CACHE_H_
#define PF_PLUGIN_LUA_CACHE_H_

#include "config.h"

enum {
  kCacheInvalid = -1,
  kCacheWaiting,
  kCacheSuccess,
  kCacheError,
}; //Cache state.

PF_PLUGIN_API int32_t dcache_get(lua_State *L);
PF_PLUGIN_API int32_t dcache_set(lua_State *L);
PF_PLUGIN_API int32_t dcache_gettable(lua_State *L);
PF_PLUGIN_API int32_t dcache_settable(lua_State *L);
PF_PLUGIN_API int32_t dcache_forget(lua_State *L);
PF_PLUGIN_API int32_t dcache_query(lua_State *L);
PF_PLUGIN_API int32_t dcache_fastforget(lua_State *L);
PF_PLUGIN_API int32_t dcache_setparam(lua_State *L);
PF_PLUGIN_API int32_t dcache_getparam(lua_State *L);
PF_PLUGIN_API int32_t dcache_setstatus(lua_State *L);
PF_PLUGIN_API int32_t dcache_getstatus(lua_State *L);
PF_PLUGIN_API int32_t dcache_test(lua_State *L);
void dcache_register(lua_State *L);

#endif //PF_PLUGIN_LUA_CACHE_H_
