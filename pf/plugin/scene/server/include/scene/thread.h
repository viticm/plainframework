/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id thread.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm@126.com>/<viticm.ti@gmail.com>
 * @date 2015/03/18 15:47
 * @uses scene thread class
 *       cn: 场景线程类，每个线程负责各自的场景
**/
#ifndef SCENE_THREAD_H_
#define SCENE_THREAD_H_

#include "scene/config.h"
#include "pf/sys/thread.h"
#include "pf/script/lua/interface.h"

#define SCENE_THREAD_PER_MAX 128

namespace scene {

class PF_PLUGIN_API Thread : public pf_sys::Thread {

 public:
   Thread();
   virtual ~Thread();

 public:
   virtual void run();
   virtual void stop();

 public:
   bool isactive() const;
   void quit();
   bool addscene(Scene *scene);
   int32_t get_scenecount() const;
   bool isinit() const;
   pf_script::lua::Interface *get_luainterface();
   int32_t get_reload_scriptid();
   void set_reload_scriptid(int32_t scriptid);

 private:
   Scene *scenes_[SCENE_THREAD_PER_MAX];
   Scene *scenesbak_[SCENE_THREAD_PER_MAX];
   pf_script::lua::Interface luainterface_;
   int32_t scenecount_;
   bool isactive_;
   uint32_t ticktimes_;
   uint32_t ticksteps_;
   bool isinit_;
   int32_t scenecount_bak_;
   pf_sys::ThreadLock lock_;
   int32_t reload_scriptid_;

};

}; //namespace scene

#endif //SCENE_THREAD_H_
