/**
 * GLOBALS["default.engine.frame"] = number;      //default 100.
 * GLOBALS["default.net.open"] = bool;            //default false.
 * GLOBALS["default.net.service"] = bool;         //default false.
 * GLOBALS["default.net.service_ip"] = string;    //default "".
 * GLOBALS["default.net.service_port"] = number;  //default 0.
 * GLOBALS["default.net.conn_max"] = number;      //default NET_CONNECTION_MAX.
 * GLOBALS["default.script.open"] = bool;         //default false.
 * GLOBALS["default.script.rootpath"] = string;   //default SCRIPT_ROOT_PATH.
 * GLOBALS["default.script.workpath"] = string;   //default SCRIPT_WORK_PATH.
 * GLOBALS["default.script.bootstrap"] = string;  //default "bootstrap.lua".
 * GLOBALS["default.script.type"] = number;       //default pf_script::kTypeLua.
 * GLOBALS["default.cache.open"] = bool;          //default fasle.
 * GLOBALS["default.cache.service"] = bool;       //default fasle.
 * GLOBALS["default.cache.conf"] = string;        //default "".
 * GLOBALS["default.cache.key_map"] = number;     //default ID_INVALID.
 * GLOBALS["default.cache.recycle_map"] = number; //default ID_INVALID.
 * GLOBALS["default.cache.query_map"] = number;   //default ID_INVALID.
 * GLOBALS["default.db.open"] = bool;             //default fasle.
 * GLOBALS["default.db.type"] = number;           //default kDBConnectorTypeODBC.
 * GLOBALS["default.db.server"] = string;         //default "".
 * GLOBALS["default.db.user"] = string;           //default "".
 * GLOBALS["default.db.password"] = string;       //default "".
 **/

#include "main.h"
#include "net.h"
#include "packet/sayhello.h"

void reload() {
  if (is_null(ENGINE_POINTER)) return;
  auto env = ENGINE_POINTER->get_script();
  if (is_null(env)) return;
  env->reload("preload.lua");
}

int32_t times = 0;
void main_loop(pf_engine::Kernel &engine) {
  std::cout << "main_loop ..." << std::endl;
  ++times;
  if (times > 10)
    std::cout << "main_loop exited by 10 times" << std::endl;
  else
    engine.enqueue([&engine](){ main_loop(engine); });
}
void main_loop1(pf_engine::Kernel &engine) {
  std::cout << "main_loop1 ..." << std::endl;
  ++times;
  if (times > 20)
    std::cout << "main_loop1 exited by 20 times" << std::endl;
  else
    engine.enqueue([&engine](){ main_loop1(engine); });
}

pf_net::connection::Basic *connector{nullptr};
void main_nconnect(pf_engine::Kernel &engine,
                   pf_net::connection::manager::Connector &mconnector) {
  mconnector.tick();
  if (is_null(connector)) {
    connector = mconnector.connect(
        "127.0.0.1", GLOBALS["default.net.service_port"].uint16());
  } else {
    static uint32_t last_time = 0;
    auto tickcount = TIME_MANAGER_POINTER->get_tickcount();
    if (tickcount - last_time > 5000) {
      SayHello packet;
      packet.set_str("hello ...");
      connector->send(&packet);
      last_time = tickcount;
    }
  }
  engine.enqueue([&engine, &mconnector](){ main_nconnect(engine, mconnector); });
}

int32_t main(int32_t argc, char * argv[]) {
  GLOBALS["app.debug"] = true;
  GLOBALS["app.name"] = "simple";
  GLOBALS["default.net.open"] = true;
  GLOBALS["default.net.service"] = true;
  GLOBALS["default.net.service_port"] = 12306;
  //GLOBALS["default.db.open"] = true;
  GLOBALS["default.script.open"] = true;
  pf_engine::Kernel engine;
  pf_engine::Application app(&engine);
  app.register_commandhandler("--reload", "lua script reload.", reload);
  engine.enqueue([](){ std::cout << "main loop function1" << std::endl; });
  engine.enqueue([&engine](){ main_loop(engine); });
  engine.enqueue([&engine](){ main_loop1(engine); });
  pf_net::connection::manager::Connector mconnector;
  init_net_packets();
  mconnector.init(1);
  engine.enqueue([&engine, &mconnector](){ main_nconnect(engine, mconnector); });
  app.run(argc, argv);
  return 0;
}
