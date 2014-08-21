#include "pf/script/lua/system.h"
#include "common/script/lua/log.h"
#include "common/script/lua/file.h"
#include "common/script/lua/export.h"

namespace common {

namespace script {

namespace lua {

int32_t luatest(lua_State *L) {
  __ENTER_FUNCTION
    lua_newtable(L);
    lua_pushstring(L, "mydata");
    lua_pushstring(L, "var");
    lua_settable(L, -3);
    return 1;
  __LEAVE_FUNCTION
    return -1;
}

bool export_globals() {
  __ENTER_FUNCTION
    if (!SCRIPT_LUASYSTEM_POINTER) return false;
    pf_script::lua::VM *vm = SCRIPT_LUASYSTEM_POINTER->get_vm();
    vm->register_function("luatest", (void *)luatest);
    if (!register_functiontable_log()) return false;
    if (!register_functiontable_file()) return false;
    return true;
  __LEAVE_FUNCTION
    return false;
}

} //namespace lua

} //namespace script

} //namespace common
