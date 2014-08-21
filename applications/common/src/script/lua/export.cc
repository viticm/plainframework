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
    for (int32_t j = 0; j < 8; ++j) {
      char basekey[20] = {0};
      snprintf(basekey, sizeof(basekey) -1, "basekey%d", j);
      lua_pushstring(L, basekey);
      lua_newtable(L);
      for (int32_t i = 0; i < 8; ++i) {
        char key[20] = {0};
        char val[20] = {0};
        snprintf(key, sizeof(key) -1, "key%d", i);
        snprintf(val, sizeof(key) - 1, "var%d", i);
        lua_pushstring(L, key);
        lua_pushstring(L, val);
        lua_settable(L, -3);
      }
      lua_settable(L, -3);
    }
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
