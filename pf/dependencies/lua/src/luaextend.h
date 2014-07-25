/**
 * PLAIN SERVER Engine ( https://github.com/viticm/plainserver )
 * $Id luaextend.h
 * @link https://github.com/viticm/plianserver for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/06/24 17:18
 * @uses script engine extend lua api
 */
#ifndef luaextend_h
#define luaextend_h

#include "lua.h"
#include "lauxlib.h"

LUA_API int lua_settable_string_fromid(lua_State* L, 
                                       int index, 
                                       int id, 
                                       const char* str);
LUA_API int lua_settable_double_fromid(lua_State* L, 
                                       int index, 
                                       int id, 
                                       double number);
LUA_API int lua_settable_int_fromid(lua_State* L, 
                                    int index, 
                                    int id, 
                                    int number);
LUA_API int lua_settable_CFunction_fromname(lua_State* L, 
                                            int index, 
                                            const char* membername, 
                                            lua_CFunction function);
LUA_API int lua_settable_CFunction_fromid(lua_State* L, 
                                          int index, 
                                          int id, 
                                          lua_CFunction function); 
LUA_API int lua_settable_string_fromname(lua_State* L, 
                                         int index, 
                                         const char * membername, 
                                         char* str);
LUA_API int lua_settable_int_fromname(lua_State* L, 
                                      int index, 
                                      const char* membername, 
                                      int number);
LUA_API int lua_settable_double_fromname(lua_State* L, 
                                         int index, 
                                         const char* membername, 
                                         double number);
LUA_API int lua_getvalues_fromstack(lua_State* L, char* format, ...);
LUA_API int lua_execute(lua_State *L);

#endif

