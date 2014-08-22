#include "pf/base/log.h"
#include "pf/file/database.h"
#include "pf/file/ini.h"
#include "pf/script/lua/system.h"
#include "common/script/lua/file.h"

#define TAB_PUSHVALUE(L, obj, line, column) { \
  switch ((obj).get_fieldtype(column)) { \
    case pf_file::Database::kTypeInt: \
      lua_pushnumber((L), (obj).search_position((line), (column))->int_value); \
      break; \
    case pf_file::Database::kTypeFloat: \
      lua_pushnumber((L), (obj).search_position((line), (column))->float_value); \
      break; \
    case pf_file::Database::kTypeString: \
      lua_pushstring((L), (obj).search_position((line), (column))->string_value); \
      break; \
    default: \
      break; \
  } \
}

namespace common {

namespace script {

namespace lua {

int32_t file_opentab(lua_State *L) {
  __ENTER_FUNCTION
    SCRIPT_LUA_CHECKARGC(L, 1);
    const char *filename = lua_tostring(L, 1);
    if (NULL == filename) return 0;
    pf_file::Database tabfile(0);
    if (!tabfile.open_from_txt(filename)) return 0;
    int32_t i, j;
    lua_newtable(L);
    int32_t tab_index = tabfile.get_fieldindex("index");
    for (i = 0; i < tabfile.get_record_number(); ++i) {
      if (-1 == tab_index) {
        lua_pushnumber(L, i);
      } else {
        TAB_PUSHVALUE(L, tabfile, i, tab_index);    
      }
      lua_newtable(L);
      for (j = 0; j < tabfile.get_field_number(); ++j) {
        const char *fieldname = tabfile.get_fieldname(j);
        lua_pushstring(L, fieldname);
        TAB_PUSHVALUE(L, tabfile, i, j);
        lua_settable(L, -3);
      }
      lua_settable(L, -3);
    }
    return 1;
  __LEAVE_FUNCTION
    return -1;
}

int32_t file_openini(lua_State *L) {
  __ENTER_FUNCTION
    SCRIPT_LUA_CHECKARGC(L, 1);
    const char *filename = lua_tostring(L, 1);
    if (NULL == filename) return 0;
    pf_file::Ini inifile(filename);
    if (inifile.get_sectionnumber() <= 0) return 0;
    int32_t i;
    int32_t sectionnumber = inifile.get_sectionnumber();
    int32_t *section_indexlist = inifile.get_section_indexlist();
    lua_newtable(L);
    for (i = 0; i < sectionnumber; ++i) {
      const char *sectionname = inifile.readstring(section_indexlist[i]);
      lua_pushstring(L, sectionname);
      lua_newtable(L);
      int32_t position = section_indexlist[i];
      for (;;) {
        position = inifile.goto_next_line(position);
        const char *key = inifile.find_key(position);
        const char *value = inifile.readstring(position);
        lua_pushstring(L, key);
        lua_pushstring(L, value);
        lua_settable(L, -3);
        if ('[' == key[0] || position >= inifile.get_datalength()) break;
      }
      lua_settable(L, -3);
    }
    return 1;
  __LEAVE_FUNCTION
    return -1;
}

static const struct luaL_Reg filetable[] = {
  {"opentab", file_opentab},
  {"openini", file_openini},
  {NULL, NULL}
};

bool register_functiontable_file() {
  __ENTER_FUNCTION
    if (!SCRIPT_LUASYSTEM_POINTER) return false;
    pf_script::lua::VM *vm = SCRIPT_LUASYSTEM_POINTER->get_vm();
    if (!vm) return false;
    vm->register_functiontable("file", filetable);
    return true;
  __LEAVE_FUNCTION
    return false;
}

} //namespace lua

} //namespace script

} //namespace common
