#include "pf/basic/io.tcc"
#include "pf/basic/time_manager.h"
#include "pf/basic/logger.h"
#include "pf/net/connection/manager/listener.h"
#include "pf/net/connection/manager/connector.h"
#include "pf/db/manager.h"
#include "pf/script/factory.h"
#include "pf/script/interface.h"
#include "pf/cache/repository.h"
#include "pf/cache/db_store.h"
#include "pf/cache/manager.h"
#include "pf/sys/thread.h"
#include "pf/engine/thread.h"
#include "pf/engine/kernel.h"

using namespace pf_engine;

template <> Kernel *pf_basic::Singleton< Kernel >::singleton_ = nullptr;

Kernel *Kernel::getsingleton_pointer() {
  return singleton_;
}

Kernel &Kernel::getsingleton() {
  Assert(singleton_);
  return *singleton_;
}

Kernel::Kernel() :
  net_{nullptr},
  db_{nullptr},
  cache_{nullptr},
  script_{nullptr},
  isinit_{false},
  stop_{false} {
}

Kernel::~Kernel() {
  for (std::thread &worker : thread_workers_) {
    worker.join();
  }
}

pf_script::Interface *Kernel::get_script() {
  if (is_null(script_)) return nullptr;
  auto env = script_->getenv(script_eid_);
  return env;
}

bool Kernel::init() {
  if (isinit_) return true;
  if (!init_base()) return false;
  if (!init_net()) return false;
  if (!init_db()) return false;
  if (!init_cache()) return false;
  if (!init_script()) return false;
  SLOW_DEBUGLOG(ENGINE_MODULENAME, "[%s] Kernel::init ok!", ENGINE_MODULENAME);
  return true;
}

void Kernel::run() {
  if (!is_null(net_)) {
    auto net = net_.get();
    this->newthread([&net]() { return thread::for_net(net); });
  }
  if (!is_null(db_)) {
    auto db = db_.get();
    this->newthread([&db]() { return thread::for_db(db); });
  }
  if (!is_null(script_) && script_eid_ != SCRIPT_EID_INVALID) { 
    auto env = script_->getenv(script_eid_);
    env->call(GLOBALS["default.script.enter"].data);
    this->newthread([&env]() { return thread::for_script(env); });
  }
  if (!is_null(cache_)) {
    auto cache = cache_.get();
    this->newthread([&cache]() { return thread::for_cache(cache); });
  }
  GLOBALS["app.status"] = kAppStatusRunning;
  loop();
}

void Kernel::stop() {
  for (std::thread &worker : thread_workers_) {
    pf_sys::thread::stop(worker);
  }
  GLOBALS["app.status"] = kAppStatusStop;
  stop_ = true;
}

bool Kernel::init_base() {
  using namespace pf_basic;
 
  io_cdebug("[%s] Kernel::init_base start...", ENGINE_MODULENAME);

  //Time manager.
  auto time_manager = new TimeManager();
  if (is_null(time_manager)) return false;
  unique_move(TimeManager, time_manager, g_time_manager);
  if (!g_time_manager->init()) return false;

  //Logger.
  auto logger = new Logger();
  if (is_null(logger)) return false;
  unique_move(Logger, logger, g_logger);
  
  return true;
}

bool Kernel::init_net() {
  using namespace pf_net;
  using namespace pf_basic;
  if (GLOBALS["default.net.open"] == false) return true;
  SLOW_DEBUGLOG(ENGINE_MODULENAME, 
                "[%s] Kernel::init_net start...", 
                ENGINE_MODULENAME);
  connection::manager::Basic *net{nullptr};
  auto conn_max = GLOBALS["default.net.conn_max"].get<uint16_t>();
  if (GLOBALS["default.net.service"] == true) {
    net = new connection::manager::Listener();
    unique_move(connection::manager::Basic, net, net_)
    auto service_ip = GLOBALS["default.net.service_ip"].c_str();
    auto service_port = GLOBALS["default.net.service_port"].get<uint16_t>();
    auto service = dynamic_cast< connection::manager::Listener *>(net);
    if (!service->init(conn_max, service_port, service_ip)) return false;
    SLOW_DEBUGLOG(ENGINE_MODULENAME,
                  "[%s] service listen at: host[%s] port[%d] conn_max[%d].",
                  ENGINE_MODULENAME,
                  service->host(),
                  service->port(),
                  conn_max);
  } else {
    net = new connection::manager::Connector();
    unique_move(connection::manager::Basic, net, net_)
    if (!net->init(conn_max)) return false;
  }
  return true;
}

bool Kernel::init_db() {
  using namespace pf_db;
  if (GLOBALS["default.db.open"] == false) return true;
  SLOW_DEBUGLOG(ENGINE_MODULENAME, 
                "[%s] Kernel::init_db start...", 
                ENGINE_MODULENAME);
  auto db = new Manager();
  unique_move(Manager, db, db_)
  if (is_null(db)) return false;
  db->set_connector_type(GLOBALS["default.db.type"].get<int8_t>());
  auto user = GLOBALS["default.db.user"].c_str();
  auto password = GLOBALS["default.db.password"].c_str();
  auto server = GLOBALS["default.db.server"].c_str();
  if (!db->init(server, user, password)) return false;
  return true;
}

bool Kernel::init_cache() {
  using namespace pf_cache;
  if (GLOBALS["default.cache.open"] == false) return true;
  SLOW_DEBUGLOG(ENGINE_MODULENAME, 
                "[%s] Kernel::init_cache start...", 
                ENGINE_MODULENAME);
  auto cache = new Manager();
  if (is_null(cache)) return false;
  unique_move(Manager, cache, cache_)
  auto dirver = cache->create_db_dirver();
  if (is_null(dirver)) return false;
  auto store = dynamic_cast< DBStore *>(dirver->store());
  auto key_map = GLOBALS["default.cache.key_map"].get<int32_t>();
  auto recycle_map = GLOBALS["default.cache.recycle_map"].get<int32_t>();
  auto query_map = GLOBALS["default.cache.query_map"].get<int32_t>();
  store->set_key(key_map, recycle_map, query_map);
  store->set_service(GLOBALS["default.cache.service"].get<bool>());
  if (!store->load_config(GLOBALS["default.cache.conf"].c_str())) return false;
  if (!store->init()) return false;
  return true;
}

bool Kernel::init_script() {
  using namespace pf_script;
  if (GLOBALS["default.script.open"] == false) return true;
  SLOW_DEBUGLOG(ENGINE_MODULENAME, 
                "[%s] Kernel::init_script start...", 
                ENGINE_MODULENAME);
  auto factory = new Factory();
  if (is_null(factory)) return false;
  unique_move(Factory, factory, script_);
  config_t conf;
  conf.rootpath = GLOBALS["default.script.rootpath"].c_str();
  conf.workpath = GLOBALS["default.script.workpath"].c_str();
  conf.type = (type_t)GLOBALS["default.script.type"].get<int8_t>();
  script_eid_ = factory->newenv(conf);
  if (SCRIPT_EID_INVALID == script_eid_) return false;
  auto env = factory->getenv(script_eid_);
  if (!env->init()) return false;
  if (!env->bootstrap(GLOBALS["default.script.bootstrap"].c_str())) 
    return false;
  return true;
}

void Kernel::loop() {
  for (;;) {
    if (GLOBALS["app.status"] == kAppStatusStop) break;
    auto starttime = TIME_MANAGER_POINTER->get_tickcount();
    std::function<void()> task;
    {
      if (!tasks_.empty()) {
        task = std::move(this->tasks_.front());
        this->tasks_.pop();
      }
    }
    if (task) task();
    worksleep(starttime);
  }
  auto check_starttime = TIME_MANAGER_POINTER->get_tickcount();
  for (;;) {
    auto diff_time = TIME_MANAGER_POINTER->get_tickcount() - check_starttime;
    if (diff_time / 1000 > 30) {
      pf_basic::io_cerr("[%s] wait thread exit exceed 30 seconds.", 
                        GLOBALS["app.name"].c_str());
      break;
    }
    if (pf_sys::ThreadCollect::count() <= 0) break;
  }
}
