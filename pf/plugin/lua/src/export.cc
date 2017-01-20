#include "file.h"
#include "logger.h"
#include "export.h"

static const struct luaL_Reg filetable[] = {
  {"opentab", file_opentab},
  {"openini", file_openini},
  {NULL, NULL}
};

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

extern "C"
int luaopen_libpf_plugin_lua(lua_State* L) {
  luaL_register(L, "file", filetable); 
  luaL_register(L, "logger", logtable);
  return 1;
}
