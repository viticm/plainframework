/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id grid.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm@126.com>/viticm.ti@gmail.com
 * @date 2015/03/16 15:32
 * @uses server scene grid class
 *       cn: 服务器场景格子类，场景地图的基本组成单位
 *       场景地图是由格子组成的，可以对某个区域的场景对象进行快速定位，同时也为了场景
 *       区域广播提供了基础
*/
#ifndef SCENE_GRID_H_
#define SCENE_GRID_H_

#include "scene/config.h"
#include "scene/define.h"
#include "structure/objectlist.h"

namespace scene {

class PF_PLUGIN_API Grid {

 public:
   Grid();
   ~Grid();

 public:
   void cleanup();
   void setid(gridid_t id);
   gridid_t getid() const;
   structure::ObjectList *get_objectlist();
   structure::ObjectList *get_humanlist();

 public:
   virtual bool on_objectenter(structure::Object *object);
   virtual bool on_objectleave(structure::Object *object);

 public:
   void addregion(const region_t *region);
   const region_t *get_currentregion(const structure::position_t *position);
   uint8_t get_regioncount() const;

 protected:
   structure::ObjectList objectlist_;
   structure::ObjectList humanlist_;
   gridid_t id_;
   uint8_t regioncount_;
   const region_t *regions_[SCENE_REGION_INZONE_MAX];

};

}; //namespace scene

#endif //SCENE_GRID_H_
