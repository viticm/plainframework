/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id region.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm@126.com>/<viticm.ti@gmail.com>
 * @date 2015/03/19 14:55
 * @uses scene region manager class
 *       cn: 场景区域管理器，注意区域是地图上划分出来方便管理的单位，
 *       与基本的格子单位不同，它们之间的关系为：格子->区域*->地图 
 *       注意区域并不是地图的基本单位，它只是地图上的某一块的划分
**/
#ifndef SCENE_MANAGER_REGION_H_
#define SCENE_MANAGER_REGION_H_

#include "scene/manager/config.h"

namespace scene {

namespace manager {

class PF_PLUGIN_API Region {

 public:
   Region();
   virtual ~Region();

 public:
   virtual bool init(const char *filename);
   void destroy();
   void setscene(scene::Scene *scene);
   uint32_t get_gridid(float x, float z);
   rect_t *get_gridrect(gridid_t gridid);

 protected:
   uint32_t count_;
   region_t *regions_;
   scene::Scene *scene_;

};

}; //namespace manager

}; //namespace scene

#endif //SCENE_MANAGER_REGION_H_
