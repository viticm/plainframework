#include "pf/net/connection/manager/basic.h"
#include "pf/db/manager.h"
#include "pf/script/interface.h"
#include "pf/script/lua/system.h"
#include "pf/cache/repository.h"
#include "pf/cache/db_store.h"
#include "pf/cache/manager.h"
#include "pf/sys/thread.h"
#include "pf/engine/thread.h"

namespace pf_engine {

namespace thread {

//Also can use object thread like std::thread t(&O::func, this, ...).
//Or by pf_sys::thread::start(t, &O::func, this, ...).

bool for_net(pf_net::connection::manager::Basic *net) {
  if (is_null(net)) return false;
  net->tick();
  return true;
}

bool for_db(pf_db::Manager *db) {
  using namespace pf_sys;
  if (is_null(db)) return false;
  db->check_db_connect();
  return true;
}

bool for_cache(pf_cache::Manager *cache) {
  if (is_null(cache)) return false;
  auto dirver = cache->get_db_dirver();
  auto store = dynamic_cast<pf_cache::DBStore *>(dirver->store());
  if (is_null(store)) return false;
  store->tick();
  return true;
}

bool for_script(pf_script::Interface *env) {
  using namespace pf_script;
  if (is_null(env)) return false;
  env->task_queue()->work_one();
#ifdef PF_OPEN_LUA
  auto lua_env = dynamic_cast< lua::System * >(env);
  if (GLOBALS["default.script.heartbeat"] != "") 
    lua_env->call(GLOBALS["default.script.heartbeat"].data);
  auto time = 
    static_cast<int32_t>(1000 / GLOBALS["default.engine.frame"].get<int32_t>());
  lua_env->gccheck(time);
#endif //PF_OPEN_LUA
  return true;
}

} //namespace thread

} //namespace pf_engine
