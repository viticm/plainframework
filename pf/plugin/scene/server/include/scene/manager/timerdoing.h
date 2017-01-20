/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id timerdoing.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm@126.com>/<viticm.ti@gmail.com>
 * @date 2015/03/19 11:08
 * @uses scene timer doing manager
 *       cn: 场景定时执行管理器
**/
#ifndef SCENE_MANAGER_TIMERDOING_H_
#define SCENE_MANAGER_TIMERDOING_H_

#include "scene/manager/config.h"

#define SCENE_MANAGER_TIMERDOING_PARAMNUM 5
#define SCENE_MANAGER_TIMERDOING_MAX 512

namespace scene {

namespace manager {

class PF_PLUGIN_API TimerDoing {

 public:
   TimerDoing();
   virtual ~TimerDoing();

 public:
   bool init();
   void cleanup();

 public:
   void start(int32_t scriptid, 
              sceneid_t sceneid, 
              int32_t index, 
              int32_t param[SCENE_MANAGER_TIMERDOING_PARAMNUM]);
   void stop(int32_t index);
   bool isvalid(int32_t index);
   void setscene(scene::Scene *scene);
   scene::Scene *getscene();

 private:
   bool is_started_[SCENE_MANAGER_TIMERDOING_MAX]; //已经开始的状态数组
   scene::Scene *scene_;

};

}; //namespace manager

}; //namespace scene

#endif //SCENE_MANAGER_TIMERDOING_H_
