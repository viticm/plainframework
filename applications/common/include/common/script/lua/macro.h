/**
 * PAP Engine ( https://github.com/viticm/pap )
 * $Id macro.h
 * @link https://github.com/viticm/pap for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm@126.com )
 * @license
 * @user viticm<viticm@126.com>
 * @date 2014/09/26 10:06
 * @uses script lua macro define
*/
#ifndef COMMON_SCRIPT_LUA_MACRO_H_
#define COMMON_SCRIPT_LUA_MACRO_H_

#include "common/script/lua/config.h"

#define SCRIPT_LUA_TABLE_TO_FUNCTIONSTRING(L,n,save,len) { \
  if (!lua_istable(L, n)) { \
    FAST_ERRORLOG(kScriptLogFile, \
                  "[script.lua] SCRIPT_LUA_TABLE_TO_FUNCTIONSTRING error," \
                  " param not a table, file: %s line: %d", \
                  __FILE__, \
                  __LINE__); \
    return -1; \
  } \
  int32_t tablesize = static_cast<int32_t>(lua_rawlen(L, 1)); \
  if (tablesize < 1) { \
    FAST_ERRORLOG(kScriptLogFile, \
                  "[script.lua] SCRIPT_LUA_TABLE_TO_FUNCTIONSTRING error," \
                  " table size < 1, file: %s line: %d", \
                  __FILE__, \
                  __LINE__); \
    return -1; \
  } \
  int32_t i; \
  for (i = 1; i <= tablesize; ++i) { \
    char value[512] = {0}; \
    pf_script::lua::gettable_string_byindex( \
        L, 1, i, value, sizeof(value) - 1); \
    if (i > 1 && !pf_script::lua::istable_numbervalue(L, 1, i)) { \
      char _value[512] = {0}; \
      snprintf(_value, sizeof(_value) - 1, "\"%s\"", value); \
      pf_base::string::safecopy(value, _value, sizeof(value)); \
    } \
    if (strstr(value, "\t")) { \
      FAST_ERRORLOG(kScriptLogFile, \
                    "[script.lua] SCRIPT_LUA_TABLE_TO_FUNCTIONSTRING error," \
                    " the param not invalid, value: %s, file: %s line: %d", \
                    value, \
                    __FILE__, \
                    __LINE__); \
      return -1; \
    } \
    if (strlen(save) + strlen(value) > len) break; \
    strncat(save, value, strlen(value)); \
    if (i < tablesize) strncat(save, "\t", 1); \
  } \
}

#endif //COMMON_SCRIPT_LUA_MACRO_H_
