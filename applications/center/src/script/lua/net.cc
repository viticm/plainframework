#include "pf/base/log.h"
#include "pf/base/string.h"
#include "pf/script/lua/lunar.h"
#include "common/define/enum.h"
#include "common/script/lua/macro.h"
#include "common/net/packet/servercenter/script_execute.h"
#include "engine/system.h"
#include "script/lua/net.h"

namespace script {

namespace lua {

int32_t net_globalexecute(lua_State *L) {
  __ENTER_FUNCTION
    using namespace common::net::packet::servercenter;
    SCRIPT_LUA_CHECKARGC(L, 1);
    char function[2048] = {0};
    SCRIPT_LUA_TABLE_TO_FUNCTIONSTRING(L, 1, function, sizeof(function) - 1);
    ScriptExecute message;
    message.set_function(function);
    ENGINE_SYSTEM_POINTER
      ->get_netmanager()
      ->broadcast(&message, kConnectionStatusGameServerReady);
    SCRIPT_LUASYSTEM_POINTER->get_vm()->callfunction(function);
    lua_pushstring(L, function);
    return 1;
  __LEAVE_FUNCTION
    return -1;
}

static const struct luaL_Reg nettable[] = { 
  {"globalexcute", net_globalexecute},
  {NULL, NULL}
};

bool register_functiontable_net() {
  __ENTER_FUNCTION
    if (!SCRIPT_LUASYSTEM_POINTER) return false;
    pf_script::lua::VM *vm = SCRIPT_LUASYSTEM_POINTER->get_vm();
    if (!vm) return false;
    vm->register_functiontable("net", nettable);
    return true;
  __LEAVE_FUNCTION
    return false;
}

} //namespace lua

} //namespace script
