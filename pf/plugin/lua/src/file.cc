#include "pf/file/tab.h"
#include "pf/file/ini.h"
#include "file.h"

#define TAB_PUSHVALUE(L, obj, line, column) { \
  switch ((obj).get_fieldtype(column)) { \
    case pf_file::Tab::kTypeInt: \
      lua_pushnumber((L), (obj).search_position((line), (column))->int_value); \
      break; \
    case pf_file::Tab::kTypeFloat: \
      lua_pushnumber((L), (obj).search_position((line), (column))->float_value); \
      break; \
    case pf_file::Tab::kTypeString: \
      lua_pushstring((L), (obj).search_position((line), (column))->string_value); \
      break; \
    default: \
      break; \
  } \
}

int32_t file_opentab(lua_State *L) {
  SCRIPT_LUA_CHECKARGC(L, 1);
  const char *filename = lua_tostring(L, 1);
  if (NULL == filename) return 0;
  pf_file::Tab tabfile(0);
  if (!tabfile.open_from_txt(filename)) return 0;
  int32_t i, j;
  lua_newtable(L);
  int32_t tab_index = tabfile.get_fieldindex("index");
  for (i = 0; i < tabfile.get_record_number(); ++i) {
    if (-1 == tab_index) {
      lua_pushnumber(L, i + 1); //lua default index from 1
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
}

int32_t file_openini(lua_State *L) {
  using namespace pf_file;
  SCRIPT_LUA_CHECKARGC(L, 1);
  const char *filename = lua_tostring(L, 1);
  if (NULL == filename) return 0;
  Ini inifile(filename);
  Ini::sectionset_t *data = inifile.getdata();
  Ini::sectionset_t::iterator it;
  lua_newtable(L);
  for (it = data->begin(); it != data->end(); ++it) {
    lua_pushstring(L, it->first.c_str());
    lua_newtable(L);
    Ini::valueset_t *values = it->second;
    Ini::valueset_t::iterator it2;
    for (it2 = values->begin(); it2 != values->end(); ++it2) {
      int32_t position = it2->second;
      const char *value = inifile.getstring(position);
      lua_pushstring(L, it2->first.c_str());
      lua_pushstring(L, value);
      lua_settable(L, -3);
    }
    lua_settable(L, -3);
  }
  return 1;
}
