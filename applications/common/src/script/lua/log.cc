#include "pf/base/log.h"
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

}

int32_t slow_warninglog(lua_State *L) {

}

int32_t slow_debuglog(lua_State *L) {

}

int32_t slow_writelog(lua_State *L) {

}

int32_t fast_log(lua_State *L) {

}

int32_t fast_errorlog(lua_State *L) {

}

int32_t fast_warninglog(lua_State *L) {

}

int32_t fast_debuglog(lua_State *L) {

}

int32_t fast_writelog(lua_State *L) {

}

} //namespace lua

} //namespace script

} //namespace common
