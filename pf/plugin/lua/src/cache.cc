#include "pf/script/lua/config.h"
#include "pf/engine/kernel.h"
#include "pf/cache/db_store.h"
#include "pf/cache/repository.h"
#include "pf/basic/io.tcc"
#include "pf/basic/stringstream.h"
#include "pf/basic/monitor.h"
#include "cache.h"

#define CACHE_PARAM_COUNT (3)
#define check_lua_argc(L,n) Assert(lua_gettop((L)) == (n))
#define dcache_lock(n,v,f) share_lock(db_item_t, *v, n, f)

#define dcache_common(L,n) using namespace pf_basic; using namespace pf_cache; \
  using namespace pf_sys::memory::share; \
  check_lua_argc(L, (n)); \
  if (!ENGINE_POINTER || !ENGINE_POINTER->get_cache()) { \
    lua_pushinteger(L, kCacheError); \
    return 1; \
  } \
  auto manager = ENGINE_POINTER->get_cache(); \
  auto cache = dynamic_cast<DBStore *>(manager->get_db_dirver()->store()); \
  auto tablename = lua_tostring(L, 1); \
  std::string cachekey{""}; \
  auto key = lua_tostring(L, 2); \
  dcache_key(tablename, key, cachekey); \
  auto dbitem = cache->getitem(cachekey);

int32_t dcache_get(lua_State *L) {
  dcache_common(L, 2)
  if (is_null(dbitem)) {
    lua_pushinteger(L, kCacheInvalid);
    return 1;
  }
  lua_pushinteger(L, kCacheSuccess);
  lua_pushstring(L, dbitem->get_data());
  return 2;
}

int32_t dcache_gettable(lua_State *L) {
  dcache_common(L, 2)
  if (is_null(dbitem)) {
    lua_pushinteger(L, kCacheInvalid);
    return 1;
  }
  if (dbitem->status != kQuerySuccess) {
    lua_pushinteger(L, kCacheWaiting);
    return 1;
  }
  char *cstr{nullptr};
  char *rstr{nullptr};
  if (!cache->get(cachekey, cstr, rstr)) {
    lua_pushinteger(L, kCacheError);
    return 1;
  }
  std::vector< std::string > names;
  std::vector< int8_t > types;
  stringstream scolumns(cstr, sizeof(db_table_info_t));
  int32_t column_count{0};
  scolumns >> column_count;
  if (0 == column_count) {
    lua_pushinteger(L, kCacheError);
    return 1;
  }
  for (size_t i = 0; i < static_cast<size_t>(column_count); ++i) {
    std::string name{""};
    int8_t type{kDBColumnTypeString};
    scolumns >> name;
    scolumns >> type;
    names.push_back(name);
    types.push_back(type);
  }
  int32_t row{0};
  stringstream srows(rstr, dbitem->size);
  srows >> row;
  if (0 == row) {
    lua_pushinteger(L, kCacheSuccess);
    return 1;
  }
  lua_pushinteger(L, kCacheSuccess);
  lua_newtable(L);
  for (uint32_t i = 0; i < static_cast<size_t>(row); ++i) {
    lua_pushnumber(L, i + 1);
    lua_newtable(L);
    for (uint32_t j = 0; j < static_cast<size_t>(column_count); ++j) {
      lua_pushstring(L, names[j].c_str());
      auto type = types[j];
      switch (type) {
        case kDBColumnTypeInteger: {
          int64_t var{0};
          srows >> var;
          lua_pushinteger(L, static_cast<lua_Integer>(var));
          break;
        }
        case kDBColumnTypeNumber: {
          double var{0};
          srows >> var;
          lua_pushnumber(L, static_cast<lua_Number>(var));
          break;
        }
        default: {
          std::string var{0};
          srows >> var;
          lua_pushstring(L, var.c_str());
          break;
        }
      }
      lua_settable(L, -3);
    }
    lua_settable(L, -3);
  }
  return 2;
}

int32_t dcache_set(lua_State *L) {
  dcache_common(L, 3);
  auto ckey = cachekey.c_str();
  auto data = lua_tostring(L, 3);
  auto datalength = strlen(data);
  if (is_null(data)) {
    lua_pushinteger(L, kCacheError);
    return 1;
  }
  if (is_null(dbitem)) {
    cache->forever(ckey, cast(void *, ""));
    dbitem = cache->getitem(cachekey);
  }
  if (is_null(dbitem)) {
    lua_pushinteger(L, kCacheError);
    return 1;
  }
  if (datalength > dbitem->size) {
    lua_pushinteger(L, kCacheError);
    return 1;
  }
  char *_data = dbitem->get_data();
  dcache_lock(auto_lock, dbitem, kFlagMixedWrite);
  cache_set(_data, data, dbitem->size);
  lua_pushinteger(L, kCacheSuccess);
  return 1;
}

static bool dcache_t2s(lua_State *L, 
                       int32_t index, 
                       const pf_cache::db_table_cinfo_t &cinfo,
                       pf_cache::db_item_t *item) {
  using namespace pf_basic;
  stringstream srows(item->get_data(), item->size);
  srows.clear();
  int32_t row{0};
  auto row_position = srows.get_position();
  srows << row;
  lua_pushnil(L); 
  if (!lua_istable(L, index)) return false;
  while (lua_next(L, index)) {
    if (!lua_istable(L, -1)) return false;
    auto sindex = lua_gettop(L);
    for (size_t i = 0; i < cinfo.names.size(); ++i) {
      lua_getfield(L, sindex, cinfo.names[i].c_str());
      auto type = cinfo.types[i];
      switch (type) {
        case kDBColumnTypeInteger: {
          auto value = static_cast<int64_t>(lua_tointeger(L, -1));
          srows << value;
          break;
        }
        case kDBColumnTypeNumber: {
          auto value = static_cast<double>(lua_tonumber(L, -1));
          srows << value;
          break;
        }
        default: {
          auto value = lua_tostring(L, -1);
          srows << value;
          break;
        }
      }
      lua_pop(L, 1);
    }
    row += 1;
    lua_pop(L, 1);
  } //while
  srows.set_position(row_position);
  srows << row;
  return true;
}

int32_t dcache_settable(lua_State *L) {
  dcache_common(L, 3);
  if (!lua_istable(L, 3)) {
    lua_pushinteger(L, kCacheError);
    return 1;
  }
  char *cstr{nullptr};
  char *rstr{nullptr};
  if (!cache->get(cachekey, cstr, rstr)) {
    lua_pushinteger(L, kCacheError);
    return 1;
  }
  db_table_cinfo_t cinfo;
  stringstream scolumns(cstr, sizeof(db_table_info_t));
  int32_t column_count{0};
  scolumns >> column_count;
  if (0 == column_count) {
    lua_pushinteger(L, kCacheError);
    return 1;
  }
  for (size_t i = 0; i < static_cast<size_t>(column_count); ++i) {
    std::string name{""};
    int8_t type{kDBColumnTypeString};
    scolumns >> name;
    scolumns >> type;
    cinfo.names.push_back(name);
    cinfo.types.push_back(type);
  }
  auto result = dcache_t2s(L, 3, cinfo, dbitem) ? kCacheSuccess : kCacheError;
  lua_pushinteger(L, result);
  return 1;
}

int32_t dcache_query(lua_State *L) {
  dcache_common(L, 2);
  if (is_null(dbitem)) {
    lua_pushinteger(L, kCacheInvalid);
    return 1;
  }
  auto result = cache->waitquery(cachekey.c_str());
  lua_pushinteger(L, result ? kCacheSuccess : kCacheError);
  return 1;
}

int32_t cache_recycle(lua_State *L) {
  dcache_common(L, 2);
  if (is_null(dbitem)) {
    lua_pushinteger(L, kCacheInvalid);
    return 1;
  }
  cache->recycle(cachekey.c_str());
  lua_pushinteger(L, kCacheSuccess);
  return 1;
}

int32_t dcache_forget(lua_State *L) {
  dcache_common(L, 2);
  if (is_null(dbitem)) {
    lua_pushinteger(L, kCacheInvalid);
    return 1;
  }
  auto ckey = cachekey.c_str();
  cache->query(ckey);
  cache->forget(ckey);
  lua_pushinteger(L, kCacheSuccess);
  return 1;
}

int32_t dcache_fastforget(lua_State *L) {
  dcache_common(L, 2);
  if (is_null(dbitem)) {
    lua_pushinteger(L, kCacheInvalid);
    return 1;
  }
  cache->forget(cachekey.c_str());
  lua_pushinteger(L, kCacheSuccess);
  return 1;
}

int32_t dcache_setparam(lua_State *L) {
  dcache_common(L, 4);
  auto param = static_cast<int8_t>(lua_tointeger(L, 3));
  auto value = static_cast<int32_t>(lua_tointeger(L, 4));
  if (param + 1 >= CACHE_PARAM_COUNT) {
    lua_pushinteger(L, kCacheError);
    return 1;
  }
  if (is_null(dbitem)) {
    lua_pushinteger(L, kCacheInvalid);
    return 1;
  }
  dcache_lock(auto_lock, dbitem, kFlagMixedWrite);
  dbitem->param[param] = value;
  lua_pushinteger(L, kCacheSuccess);
  return 1;
}

int32_t dcache_getparam(lua_State *L) {
  dcache_common(L, 3);
  auto param = static_cast<int8_t>(lua_tointeger(L, 3));
  if (param + 1 >= CACHE_PARAM_COUNT) {
    lua_pushinteger(L, kCacheError);
    return 1;
  }
  if (is_null(dbitem)) {
    lua_pushinteger(L, kCacheInvalid);
    return 1;
  }
  dcache_lock(auto_lock, dbitem, kFlagMixedRead);
  int32_t value = dbitem->param[param];
  lua_pushinteger(L, kCacheSuccess);
  lua_pushinteger(L, value);
  return 2;
}

int32_t dcache_setstatus(lua_State *L) {
  dcache_common(L, 3);
  auto status = static_cast<int8_t>(lua_tointeger(L, 3));
  if (is_null(dbitem)) {
    lua_pushinteger(L, kCacheInvalid);
    return 1;
  }
  dcache_lock(auto_lock, dbitem, kFlagMixedWrite);
  dbitem->status = status;
  lua_pushinteger(L, kCacheSuccess);
  return 1;
}

int32_t dcache_getstatus(lua_State *L) {
  dcache_common(L, 2);
  if (is_null(dbitem)) {
    lua_pushinteger(L, kCacheInvalid);
    return 1;
  }
  dcache_lock(auto_lock, dbitem, kFlagMixedRead);
  int32_t value = dbitem->status;
  lua_pushinteger(L, kCacheSuccess);
  lua_pushinteger(L, value);
  return 2;
}

int32_t dcache_test(lua_State *L) {
  auto type = lua_tointeger(L, 1);
  pf_basic::io_cerr("type: %d", type);
  lua_pushnil(L); 
  if (lua_istable(L, 2)) {
    while (lua_next(L, 2)) {
      pf_basic::io_cerr("lua_next L,2");
      if (lua_istable(L, -1)) {
        auto index = lua_gettop(L);
        pf_basic::io_cerr("index: %d", index);
        lua_pushnil(L);
        while (lua_next(L, index)) {
          auto key = lua_tostring(L, -2);
          auto str = lua_tostring(L, -1);
          pf_basic::io_cerr("key: %s, str: %s", key, str);
          lua_pop(L, 1);
        }
      }
      lua_pop(L, 1);
    }
  }
  return 0;
}

static const struct luaL_Reg dcachetable[] = {
  {"get", dcache_get},
  {"set", dcache_set},
  {"gettable", dcache_gettable},
  {"settable", dcache_settable},
  {"forget", dcache_forget},
  {"query", dcache_query},
  {"test", dcache_test},
  {"fastforget", dcache_fastforget},
  {"setparam", dcache_setparam},
  {"getparam", dcache_getparam},
  {"setstatus", dcache_setstatus},
  {"getstatus", dcache_getstatus},
  {NULL, NULL}
};

#define pushinteger(L,n,v) lua_getglobal(L, (n)); \
  lua_pushinteger(L, (v)); \
  lua_setglobal(L, (n));

void dcache_register(lua_State *L) {
  using namespace pf_cache;
  luaL_register(L, "dcache", dcachetable);
  pushinteger(L, "kQueryInvalid", kQueryInvalid);
  pushinteger(L, "kQueryError", kQueryError);
  pushinteger(L, "kQueryWaiting", kQueryWaiting);
  pushinteger(L, "kQueryInsert", kQueryInsert);
  pushinteger(L, "kQueryDelete", kQueryDelete);
  pushinteger(L, "kQuerySelect", kQuerySelect);
  pushinteger(L, "kQueryUpdate", kQueryUpdate);
  pushinteger(L, "kQuerySuccess", kQuerySuccess);
  pushinteger(L, "kCacheInvalid", kCacheInvalid);
  pushinteger(L, "kCacheWaiting", kCacheWaiting);
  pushinteger(L, "kCacheSuccess", kCacheSuccess);
  pushinteger(L, "kCacheError", kCacheError);
}
