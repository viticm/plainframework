#include "pf/basic/logger.h"
#include "logger.h"

#define SCRIPT_MODULENAME "script"

int32_t slow_log(lua_State *L) {
  SCRIPT_LUA_CHECKARGC(L, 1);
  const char *logstr = lua_tostring(L, 1);
  SLOW_LOG(SCRIPT_MODULENAME, logstr);
  return 0;
}

int32_t slow_errorlog(lua_State *L) {
  SCRIPT_LUA_CHECKARGC(L, 1);
  const char *logstr = lua_tostring(L, 1);
  SLOW_ERRORLOG(SCRIPT_MODULENAME, logstr);
  return 0;
}

int32_t slow_warninglog(lua_State *L) {
  SCRIPT_LUA_CHECKARGC(L, 1);
  const char *logstr = lua_tostring(L, 1);
  SLOW_WARNINGLOG(SCRIPT_MODULENAME, logstr);
  return 0;
}

int32_t slow_debuglog(lua_State *L) {
  SCRIPT_LUA_CHECKARGC(L, 1);
  const char *logstr = lua_tostring(L, 1);
  SLOW_DEBUGLOG(SCRIPT_MODULENAME, logstr);
  return 0;
}

int32_t slow_writelog(lua_State *L) {
  SCRIPT_LUA_CHECKARGC(L, 1);
  const char *logstr = lua_tostring(L, 1);
  SLOW_WRITELOG(SCRIPT_MODULENAME, logstr);
  return 0;
}

int32_t fast_log(lua_State *L) {
  SCRIPT_LUA_CHECKARGC(L, 1);
  const char *logstr = lua_tostring(L, 1);
  FAST_LOG(SCRIPT_MODULENAME, logstr);
  return 0;
}

int32_t fast_errorlog(lua_State *L) {
  SCRIPT_LUA_CHECKARGC(L, 1);
  const char *logstr = lua_tostring(L, 1);
  FAST_ERRORLOG(SCRIPT_MODULENAME, logstr);
  return 0;
}

int32_t fast_warninglog(lua_State *L) {
  SCRIPT_LUA_CHECKARGC(L, 1);
  const char *logstr = lua_tostring(L, 1);
  FAST_WARNINGLOG(SCRIPT_MODULENAME, logstr);
  return 0;
}

int32_t fast_debuglog(lua_State *L) {
  SCRIPT_LUA_CHECKARGC(L, 1);
  const char *logstr = lua_tostring(L, 1);
  FAST_DEBUGLOG(SCRIPT_MODULENAME, logstr);
  return 0;
}

int32_t fast_writelog(lua_State *L) {
  SCRIPT_LUA_CHECKARGC(L, 1);
  const char *logstr = lua_tostring(L, 1);
  FAST_WRITELOG(SCRIPT_MODULENAME, logstr);
  return 0;
}
