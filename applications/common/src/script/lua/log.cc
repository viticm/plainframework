#include "pf/base/log.h"
#include "pf/script/lua/system.h"
#include "pf/script/lua/config.h"
#include "common/script/lua/log.h"

namespace common {

namespace script {

namespace lua {

int32_t slow_log(lua_State *L) {
  __ENTER_FUNCTION
    SCRIPT_LUA_CHECKARGC(L, 1);
    const char *logstr = lua_tostring(L, 1);
    SLOW_LOG(SCRIPT_MODULENAME, logstr);
    return 0;
  __LEAVE_FUNCTION
    return -1;
}

int32_t slow_errorlog(lua_State *L) {
  __ENTER_FUNCTION
    SCRIPT_LUA_CHECKARGC(L, 1);
    const char *logstr = lua_tostring(L, 1);
    SLOW_ERRORLOG(SCRIPT_MODULENAME, logstr);
    return 0;
  __LEAVE_FUNCTION
    return -1;
}

int32_t slow_warninglog(lua_State *L) {
  __ENTER_FUNCTION
    SCRIPT_LUA_CHECKARGC(L, 1);
    const char *logstr = lua_tostring(L, 1);
    SLOW_WARNINGLOG(SCRIPT_MODULENAME, logstr);
    return 0;
  __LEAVE_FUNCTION
    return -1;
}

int32_t slow_debuglog(lua_State *L) {
  __ENTER_FUNCTION
    SCRIPT_LUA_CHECKARGC(L, 1);
    const char *logstr = lua_tostring(L, 1);
    SLOW_DEBUGLOG(SCRIPT_MODULENAME, logstr);
    return 0;
  __LEAVE_FUNCTION
    return -1;
}

int32_t slow_writelog(lua_State *L) {
  __ENTER_FUNCTION
    SCRIPT_LUA_CHECKARGC(L, 1);
    const char *logstr = lua_tostring(L, 1);
    SLOW_WRITELOG(SCRIPT_MODULENAME, logstr);
    return 0;
  __LEAVE_FUNCTION
    return -1;
}

int32_t fast_log(lua_State *L) {
  __ENTER_FUNCTION
    SCRIPT_LUA_CHECKARGC(L, 1);
    const char *logstr = lua_tostring(L, 1);
    FAST_LOG(kScriptLogFile, logstr);
    return 0;
  __LEAVE_FUNCTION
    return -1;
}

int32_t fast_errorlog(lua_State *L) {
  __ENTER_FUNCTION
    SCRIPT_LUA_CHECKARGC(L, 1);
    const char *logstr = lua_tostring(L, 1);
    FAST_ERRORLOG(kScriptLogFile, logstr);
    return 0;
  __LEAVE_FUNCTION
    return -1;
}

int32_t fast_warninglog(lua_State *L) {
  __ENTER_FUNCTION
    SCRIPT_LUA_CHECKARGC(L, 1);
    const char *logstr = lua_tostring(L, 1);
    FAST_WARNINGLOG(kScriptLogFile, logstr);
    return 0;
  __LEAVE_FUNCTION
    return -1;
}

int32_t fast_debuglog(lua_State *L) {
  __ENTER_FUNCTION
    SCRIPT_LUA_CHECKARGC(L, 1);
    const char *logstr = lua_tostring(L, 1);
    FAST_DEBUGLOG(kScriptLogFile, logstr);
    return 0;
  __LEAVE_FUNCTION
    return -1;
}

int32_t fast_writelog(lua_State *L) {
  __ENTER_FUNCTION
    SCRIPT_LUA_CHECKARGC(L, 1);
    const char *logstr = lua_tostring(L, 1);
    FAST_WRITELOG(kScriptLogFile, logstr);
    return 0;
  __LEAVE_FUNCTION
    return -1;
}

static const struct luaL_Reg logtable[] = {
  {"slow", slow_log},
  {"slow_error", slow_errorlog},
  {"slow_warning", slow_warninglog},
  {"slow_debug", slow_debuglog},
  {"slow_write", slow_writelog},
  {"fast", fast_log},
  {"fast_error", fast_errorlog},
  {"fast_warning", fast_warninglog},
  {"fast_debug", fast_debuglog},
  {"fast_write", fast_writelog},
  {NULL, NULL}
};

bool register_functiontable_log() {
  __ENTER_FUNCTION
    if (!SCRIPT_LUASYSTEM_POINTER) return false;
    pf_script::lua::VM *vm = SCRIPT_LUASYSTEM_POINTER->get_vm();
    if (!vm) return false;
    vm->register_functiontable("log", logtable);
    return true;
  __LEAVE_FUNCTION
    return false;
}

} //namespace lua

} //namespace script

} //namespace common
