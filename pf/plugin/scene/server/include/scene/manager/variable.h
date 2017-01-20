/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id variable.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm@126.com>/<viticm.ti@gmail.com>
 * @date 2015/03/19 15:23
 * @uses scene variable manager class
 *       cn: 场景变量管理器，场景上的动态数据主要是为了让场景拥有不同的动态属性
**/
#ifndef SCENE_MANAGER_VARIABLE_H_
#define SCENE_MANAGER_VARIABLE_H_

#include "scene/manager/config.h"

namespace scene {

namespace manager {

class PF_PLUGIN_API Variable {

 public:
   Variable();
   ~Variable();

 public:
   bool init(const variable_t &variable);
   void setscene(scene::Scene *scene);
   int32_t get(int32_t index);
   void set(int32_t index, int32_t value);
   bool heartbeat(uint32_t time);
 
 private:
   variableopt_t variableopt_;
   scene::Scene *scene_;

};

}; //namespace manager

}; //namespace scene

#endif //SCENE_MANAGER_VARIABLE_H_
