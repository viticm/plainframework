/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id timer.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm@126.com>/viticm.ti@gmail.com
 * @date 2015/03/16 17:33
 * @uses server scene timer class
 *       cn: 场景定时器，场景自身的定时器，处理整个场景的定时逻辑（脚本）
*/
#ifndef SCENE_TIMER_H_
#define SCENE_TIMER_H_

#include "scene/config.h"
#include "pf/base/tinytimer.h"

namespace scene {

class PF_PLUGIN_API Timer {

 public:
   Timer();
   ~Timer();

 public:
   pf_base::TinyTimer refeshtimer_;
   enum {kScriptFunctionNameLength = 32};
   typedef struct timer_struct {
     char script_functionname[kScriptFunctionNameLength];
     int32_t id;
     int32_t scriptid;
     pf_base::TinyTimer timer;
   } timer_t;
   timer_t *timers_;
   timer_t *system_timers_;
   Scene *scene_;
   int32_t timercount_max_;
   bool dumptimers_;
 
 public:
   void cleanup();
   bool isinit() const;
   void init(int32_t countmax, Scene *scene);
   void normalinit(int32_t index);
   int32_t get();
   void free(int32_t index);
   bool check(int32_t index);
   void normalstart(int32_t index, 
                    int32_t id, 
                    int32_t scriptid, 
                    const char *functionname, 
                    int32_t ticktime);
   void systeminit(int32_t index);
   void systemstart(int32_t index, 
                    int32_t scriptid, 
                    const char *functionname, 
                    int32_t ticktime);
   int32_t system_remaintime(int32_t index);
   void onevent(uint32_t time);
   Scene *getscene();

};

}; //namespace scene

#endif //SCENE_TIMER_H_
