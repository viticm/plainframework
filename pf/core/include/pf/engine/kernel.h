/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id kernel.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/06/30 15:43
 * @uses the plian server engine kernel class
 */
#ifndef PF_ENGINE_KERNEL_H_
#define PF_ENGINE_KERNEL_H_

#include "pf/engine/config.h"
#include "pf/base/hashmap/template.h"
#include "pf/db/manager.h"
#include "pf/net/manager.h"
#include "pf/engine/thread/db.h"
#include "pf/engine/thread/net.h"
#include "pf/engine/thread/performance.h"
#include "pf/engine/thread/script.h"

namespace pf_engine {

class PF_API Kernel {

 public:
   Kernel();
   ~Kernel();

 public: //kernel sys functions
   bool init();
   void run();
   void stop();
   void registerconfig(int32_t key, int32_t value);
   void registerconfig(int32_t key, bool value);
   void registerconfig(int32_t key, const char *value);
   bool setconfig(int32_t key, int32_t value);
   bool setconfig(int32_t key, bool value);
   bool setconfig(int32_t key, const char *value);
   int32_t getconfig_int32value(int32_t key);
   bool getconfig_boolvalue(int32_t key);
   const char *getconfig_stringvalue(int32_t key);

 public: //kernel for set_* functions
   void set_base_logprint(bool flag);
   void set_base_logactive(bool flag);
   void set_net_stream_usepacket(bool flag);

 protected:
   pf_base::hashmap::Template<int32_t, const char *> config_string_;
   pf_base::hashmap::Template<int32_t, int32_t> config_int32_;
   pf_base::hashmap::Template<int32_t, bool> config_bool_;
   pf_db::Manager *db_manager_;
   pf_net::Manager *net_manager_;
   thread::DB *db_thread_;
   thread::Net *net_thread_;
   thread::Performance *performance_thread_;
   thread::Script *script_thread_;
   bool isactive_;
   uint32_t FPS_; //帧率

 protected:
   //子类重载此方法以实现初始化不同类型的网络连接池，
   //引擎默认连接的socket未初始化，只在接受新连接时才初始化，
   //如需服务器启动就初始化可以重写此方法来实现（启动时需要足够的内存）。
   virtual bool init_net_connectionpool();
   virtual bool loop_handle(); //引擎循环处理的逻辑，受帧率控制

 protected:
   bool init_base();
   bool init_db();
   bool init_net();
   bool init_script();
   bool init_performance();
   void run_base();
   void run_db();
   void run_net();
   void run_script();
   void run_performance();
   void stop_base();
   void stop_db();
   void stop_net();
   void stop_script();
   void stop_performance();

 private:
   void calculate_FPS(); //计算帧率

};

}; //namespace pf_engine

#endif //PF_ENGINE_KERNEL_H_
