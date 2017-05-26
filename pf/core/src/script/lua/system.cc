#include "pf/basic/logger.h"
#include "pf/basic/util.h"
#include "pf/basic/time_manager.h"
#include "pf/sys/memory/dynamic_allocator.h"
#include "pf/script/lua/system.h"

#ifdef PF_OPEN_LUA

using namespace pf_script::lua;

bool System::init() {
  if (0 == stack_size_) {
#if LUA_VERSION_NUM >= 502
    stack_size_ = 1024 * 100;
#else
    stack_size_ = 1024 * 4;
#endif
  }
  lua_state_ = is_null(lua_state_) ? luaL_newstate() : lua_state_;
  if (is_null(lua_state_)) {
    on_scripterror(kErrorCodeCreate);
    return false;
  }
  auto size = static_cast<int32_t>(stack_size_);
  if (lua_checkstack(lua_state_, size) != 1) {
    on_scripterror(kErrorCodeResize);
    return false;
  }
  open_libs();
  setglobal("ROOTPATH", filebridge_.get_rootpath());
  setglobal("WORKPATH", filebridge_.get_rootpath());
#if OS_UNIX
  setglobal("OS_UNIX", true);
#elif OS_WIN
  setglobal("OS_WIN", true);
#endif
  return true;
}

bool System::bootstrap(const std::string &filename) {
  return load(filename);
}

void System::release() {
  unregister_refs();
  if (is_null(lua_state_)) return;
  lua_close(lua_state_);
  lua_state_ = nullptr;
}

bool System::load(const std::string &filename) {
  using namespace pf_sys;
  if (is_null(lua_state_)) return false;
#if LUA_VERSION_NUM >= 900 //This way has some error(loadfile no error code).
  char fullpath[FILENAME_MAX]{0};
  filebridge_.get_fullpath(fullpath, filename.c_str(), sizeof(fullpath) - 1);
  SLOW_DEBUGLOG("test", "full: %s", fullpath);
  luaL_dofile(lua_state_, fullpath);
#else
  uint64_t size;
  if (!filebridge_.open(filename.c_str())) {
    SLOW_ERRORLOG(SCRIPT_MODULENAME,
                  "[script.lua] (System::load) open file %s failed",
                  filename.c_str());
    return false;
  }
  size = filebridge_.size();
  memory::DynamicAllocator memory;
  if (!memory.malloc(static_cast<size_t>(size + 4))) {
    SLOW_ERRORLOG(SCRIPT_MODULENAME,
                  "[script.lua] (System::load) memory for file %s failed",
                  filename.c_str());
    filebridge_.close();
    return false;
  }
  if (filebridge_.read(memory.get(), size) != size) {
    SLOW_ERRORLOG(SCRIPT_MODULENAME,
                  "[script.lua] (System::load) read file %s failed",
                  filename.c_str());
    filebridge_.close();
    return false;
  }
  char *memory_pointer = reinterpret_cast<char *>(memory.get());
  memory_pointer[size + 1] = '\0';
  filebridge_.close();
  try {
    const char *temp_pointer = 
      reinterpret_cast<const char *>(memory.get());
    if (!loadbuffer(temp_pointer, size)) {
      SLOW_ERRORLOG(SCRIPT_MODULENAME,
                    "[script.lua] (System::load) load file %s"
                    " buffer cache failed",
                    filename.c_str());
      return false;
    }
  } catch (...) {
    SLOW_ERRORLOG(SCRIPT_MODULENAME,
                  "[script.lua] (System::load) load file %s"
                  " buffer cache have a exception",
                  filename.c_str());
    return false;
  }
  if (!executecode()) {
    SLOW_ERRORLOG(SCRIPT_MODULENAME,
                  "[script.lua] (System::load) execute code"
                  " failed from file %s",
                  filename.c_str());
    return false;
  }
#endif //LUA_VERSION_NUM >= 500
  return true;
}

void System::setglobal(const std::string &name, const pf_basic::type::variable_t &var) {
  using namespace pf_basic;
  if (is_null(lua_state_)) return;
  lua_getglobal(lua_state_, name.c_str());
  switch (var.type) {
    case type::kVariableTypeBool:
      lua_pushboolean(lua_state_, var.get<bool>());
      break;
    case type::kVariableTypeString:
      lua_pushstring(lua_state_, var.c_str());
      break;
    case type::kVariableTypeInt32:
    case type::kVariableTypeUint32:
    case type::kVariableTypeInt16:
    case type::kVariableTypeUint16:
    case type::kVariableTypeInt8:
    case type::kVariableTypeUint8:
      lua_pushinteger(lua_state_, var.get<int32_t>());
      break;
    case type::kVariableTypeInt64:
    case type::kVariableTypeUint64:
    case type::kVariableTypeFloat:
    case type::kVariableTypeDouble:
      lua_pushnumber(lua_state_, var.get<double>());
      break;
    default:
      lua_pushnil(lua_state_);
      break;
  }
  lua_setglobal(lua_state_, name.c_str());
}

void System::getglobal(const std::string &name, pf_basic::type::variable_t &var) {
  if (is_null(lua_state_)) return;
  lua_getglobal(lua_state_, name.c_str());
  if (1 == lua_isnumber(lua_state_, -1)) {
    var = lua_tonumber(lua_state_, -1);
  } else if (1 == lua_isstring(lua_state_, -1)) {
    var = lua_tostring(lua_state_, -1);
  }
  lua_pop(lua_state_, 1);
}

bool System::call(const std::string &str) {
  using namespace pf_basic;
  int32_t result = 1;
  if (!lua_state_) {
    on_scripterror(kErrorCodeStateIsNil);
    return false;
  }
  std::vector<std::string> array;
  string::explode(str.c_str(), array, "\t", true, true);
  if (array.size() < 1) return false;
  std::vector<std::string> _array;
  string::explode(array[0].c_str(), _array, ".", true, true);
  if (2 == _array.size()) {
    const char *table = _array[0].c_str();
    const char *field = _array[1].c_str();
    if (!get_ref(table, field)) register_ref(table, field);
    if (!get_ref(table, field)) {
      SLOW_ERRORLOG(SCRIPT_MODULENAME,
                    "[script.lua] (System::call) can't get ref,"
                    " string: %s, table: %s, field: %s",
                    str.c_str(),
                    table,
                    field);
      return false;
    }
  } else {
    lua_getglobal(lua_state_, array[0].c_str());
    if (lua_isnil(lua_state_, -1)) {
      SLOW_ERRORLOG(SCRIPT_MODULENAME,
                    "[script.lua] (System::call) can't get global value,"
                    " function: %s, string: %s",
                    str.c_str(),
                    array[0].c_str());
      return false;
    }
  }
  for (int32_t i = 1; i < static_cast<int32_t>(array.size()); ++i) {
    char value[512] = {0};
    string::safecopy(value, array[i].c_str(), sizeof(value));
    if ('\"' == value[0] && 
        '\"' == value[strlen(value) - 1]) {
      value[strlen(value) - 1] = '\0';
      lua_pushstring(lua_state_, value + 1);
    } else {
      lua_pushnumber(lua_state_, 
                     static_cast<lua_Number>(atof(value)));
    }
  }
  int32_t call_result = 
    lua_pcall(lua_state_, (int32_t)array.size() - 1, result, 0);
  if (call_result != 0) {
    on_scripterror(kErrorCodeExecute, result);
    return false;
  }
  return true;
}

bool System::call(const std::string &name, 
                  const pf_basic::type::variable_array_t &params,
                  pf_basic::type::variable_array_t &results) {
  using namespace pf_basic;
  if (!lua_state_) {
    on_scripterror(kErrorCodeStateIsNil);
    return false;
  }
  lua_getglobal(lua_state_, name.c_str());
  uint32_t paramcount = static_cast<uint32_t>(params.size());
  uint32_t resultcount = static_cast<uint32_t>(results.size());
  for (uint32_t i = 0; i < paramcount; ++i) {
    type::var_t type = params[i].type;
    switch (type) {
      case type::kVariableTypeInvalid:
        lua_pushnil(lua_state_);
        break;
      case type::kVariableTypeString:
        lua_pushstring(lua_state_, params[i].c_str());
        break;
      case type::kVariableTypeInt32:
      case type::kVariableTypeUint32:
      case type::kVariableTypeInt16:
      case type::kVariableTypeUint16:
      case type::kVariableTypeInt8:
      case type::kVariableTypeUint8:
        lua_pushinteger(lua_state_, params[i].get<int32_t>());
        break;
      case type::kVariableTypeBool:
        lua_pushboolean(lua_state_, params[i].get<bool>());
        break;
      default:
        lua_pushnumber(lua_state_, params[i].get<double>());
        break;
    }
  }
  int32_t call_result = lua_pcall(lua_state_, paramcount, resultcount, 0);
  if (call_result != 0) {
    on_scripterror(kErrorCodeExecute, resultcount);
    return false;
  } else {
    for (uint32_t i = 0; i < resultcount; ++i) {
      type::var_t type = results[i].type;
      switch (type) {
        case type::kVariableTypeString:
          results[i] = lua_tostring(lua_state_, i - 1);
          break;
        case type::kVariableTypeBool:
          results[i] = lua_toboolean(lua_state_, i - 1);
          break;
        default:
          results[i] = lua_tonumber(lua_state_, i - 1);
          break;
      }
    }
  }
  return true;
}

bool System::loadbuffer(const char *buffer, size_t size) {
 if (luaL_loadbuffer(lua_state_, 
                      buffer, 
                      size, 
                      NULL) != 0) {
    on_scripterror(kErrorCodeLoadBuffer);
    return false;
  }
  return true;
}

bool System::register_table(
    const char *name, const struct luaL_Reg regtable[]) {
  if (is_null(lua_state_)) return false;
  lua_getglobal(lua_state_, name);
  if (lua_isnil(lua_state_, -1)) lua_newtable(lua_state_); //没有才创建
  for (; regtable->name != NULL; ++regtable) {
    lua_pushstring(lua_state_, regtable->name);
    lua_pushcfunction(lua_state_, regtable->func);
    lua_settable(lua_state_, -3);
  }
  lua_setglobal(lua_state_, name);
  return true;
}

bool System::register_ref(const std::string &table, const std::string &field) {
  lua_getglobal(lua_state_, table.c_str());
  if (lua_isnil(lua_state_, -1 )) return false;
  lua_getfield(lua_state_, -1, field.c_str());
  if (lua_isnil(lua_state_, -1 )) return false;
  pf_basic::type::variable_t key = table;
  key += ".";
  key += field;
  if (refs_.find(key.c_str()) != refs_.end()) return false;
  int32_t index = luaL_ref(lua_state_, LUA_REGISTRYINDEX);
  refs_[key.c_str()] = index;
  lua_pop(lua_state_, 1);
  return true;
}

bool System::get_ref(const std::string &table, const std::string &field) {
  pf_basic::type::variable_t key = table;
  key += ".";
  key += field;
  if (refs_.find(key.c_str()) == refs_.end()) return false;
  int32_t index = refs_[key.c_str()];
  if (LUA_NOREF == index) return false;
  lua_rawgeti(lua_state_, LUA_REGISTRYINDEX, index);
  if (lua_isnil(lua_state_, -1 )) return false;
  return true;
}

bool System::unregister_ref(const std::string &table, const std::string &field) {
  pf_basic::type::variable_t key = table;
  key += ".";
  key += field;
  if (refs_.find(key.c_str()) == refs_.end()) return false;
  int32_t index = refs_[key.c_str()];
  if (LUA_NOREF == index) return false;
  luaL_unref(lua_state_, LUA_REGISTRYINDEX, index);
  refs_.erase(refs_.find(key.c_str()));
  return true;
}

void System::unregister_refs() {
  std::map<std::string, int32_t>::iterator _iterator;
  for (_iterator = refs_.begin(); _iterator != refs_.end(); ++_iterator) {
    luaL_unref(lua_state_, LUA_REGISTRYINDEX, _iterator->second);
  }
  refs_.clear();
}

void System::gccheck(int32_t freetime) {
  if (!lua_state_) return;
  int32_t delta = 0;
  int32_t turn = 0;
  uint32_t memorycount1;
  uint32_t memorycount2;
  int32_t havetime = freetime;
  int32_t reclaim = 0;
  uint32_t start_tickcount = TIME_MANAGER_POINTER->get_tickcount();
  for (turn = 0; turn < 3; ++turn) {
    memorycount1 = lua_gc(lua_state_, LUA_GCCOUNT, 0);
    reclaim = havetime * 1;
    if (1 == lua_gc(lua_state_, LUA_GCSTEP, reclaim)) {
      lua_gc(lua_state_, LUA_GCRESTART, 0);
    }
    memorycount2 = lua_gc(lua_state_, LUA_GCCOUNT, 0);
    delta += memorycount1 - memorycount2;
    uint32_t current_tickcount = TIME_MANAGER_POINTER->get_tickcount();
    havetime -= (current_tickcount - start_tickcount);
    if (havetime < 40 || delta <= reclaim) break;
  }
  if (delta > 1024 * 1024 * 500) {
    char temp[128] = {0};
    uint64_t size = static_cast<uint64_t>(delta);
    pf_basic::util::get_sizestr(size, temp, sizeof(temp) - 1);
    FAST_DEBUGLOG(SCRIPT_MODULENAME,
                  "[script.lua] System::checkgc success,"
                  " freetime: %d, memory: %s",
                  freetime,
                  temp);
  }
}

void System::setfield(
    const std::string &table, const std::string &field, const pf_basic::type::variable_t &var) {
  using namespace pf_basic;
  lua_getglobal(lua_state_, table.c_str());
  if (lua_isnil(lua_state_, -1)) lua_newtable(lua_state_);
  if (lua_istable(lua_state_, -1) != 1) return;
  lua_pushstring(lua_state_, field.c_str());
  switch (var.type) {
    case type::kVariableTypeBool:
      lua_pushboolean(lua_state_, var.get<bool>());
      break;
    case type::kVariableTypeString:
      lua_pushstring(lua_state_, var.c_str());
      break;
    case type::kVariableTypeInt32:
    case type::kVariableTypeUint32:
    case type::kVariableTypeInt16:
    case type::kVariableTypeUint16:
    case type::kVariableTypeInt8:
    case type::kVariableTypeUint8:
    case type::kVariableTypeInt64:
    case type::kVariableTypeUint64:
    case type::kVariableTypeFloat:
    case type::kVariableTypeDouble:
      lua_pushnumber(lua_state_, var.get<double>());
      break;
    default:
      lua_pushnil(lua_state_);
      break;
  }
  lua_settable(lua_state_, -3);
  lua_setglobal(lua_state_, table.c_str());
}

void System::getfield(
    const std::string &table, const std::string &field, pf_basic::type::variable_t &var) {
  if (is_null(lua_state_)) return;
  lua_getglobal(lua_state_, table.c_str());
  if (lua_istable(lua_state_, -1) != 1) return;
  lua_getfield(lua_state_, -1, field.c_str());
  if (1 == lua_isnumber(lua_state_, -1)) {
    var = lua_tonumber(lua_state_, -1);
  } else if (1 == lua_isstring(lua_state_, -1)) {
    var = lua_tostring(lua_state_, -1);
  }
  lua_pop(lua_state_, 1);
}

bool System::executecode() {
  if (is_null(lua_state_)) {
    on_scripterror(kErrorCodeExecute);
    return false;
  }
  auto state = lua_pcall(lua_state_, 0, LUA_MULTRET, 0);
  if (state != 0) {
    on_scripterror(kErrorCodeExecute, state);
    return false;
  }
  return true;
}

void System::open_libs() {
  if (is_null(lua_state_)) return;
  luaopen_base(lua_state_);
  luaL_openlibs(lua_state_);
}

void System::on_scripterror(int32_t error) {
  auto err_str = lua_tostring(lua_state_, lua_gettop(lua_state_));
  SLOW_ERRORLOG(SCRIPT_MODULENAME,
                "[script.lua] (System::on_scripterror) code: %d, message: %s",
                error,
                err_str);
}

void System::on_scripterror(int32_t error1, int32_t error2) {
  auto err_str = lua_tostring(lua_state_, lua_gettop(lua_state_));
  SLOW_ERRORLOG(SCRIPT_MODULENAME,
                "[script.lua] (System::on_scripterror) code: %d[%d], message: %s",
                error1,
                error2, 
                err_str);
}

#endif //PF_OPEN_LUA
