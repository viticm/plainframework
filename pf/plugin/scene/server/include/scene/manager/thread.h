/**
 * PAP Engine ( https://github.com/viticm/pap )
 * $Id thread.h
 * @link https://github.com/viticm/pap for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm@126.com )
 * @license
 * @user viticm<viticm@126.com>
 * @date 2015/03/25 17:57
 * @uses scene thread manager class
 *       cn: 场景线程管理器
*/
#ifndef SCENE_MANAGER_THREAD_H_
#define SCENE_MANAGER_THREAD_H_

#include "scene/manager/config.h"
#include "pf/sys/thread.h"

namespace scene {

namespace manager {

class PF_PLUGIN_API Thread {

 public:
   Thread();
   virtual ~Thread();

 public:
   virtual bool init(settings_t &settings, int32_t serverid, uint16_t countmax);
   virtual bool start();
   virtual bool stop();

 public:
   pf_sys::ThreadPool *getpool();
   uint16_t getcount() const;
   bool isinit() const;

 protected:
   uint16_t count_;
   pf_sys::ThreadPool *pool_;
   bool isinit_;

};

}; //namespace manager

}; //namespace scene

#endif //SCENE_MANAGER_THREAD_H_
