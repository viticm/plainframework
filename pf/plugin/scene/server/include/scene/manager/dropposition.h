/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id dropposition.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm@126.com>/<viticm.ti@gmail.com>
 * @date 2015/03/19 14:02
 * @uses scene drop position manager class
 *       cn: 掉落点管理器，物品掉落点管理
 *       掉落点是指怪物或玩家可掉落对象产生的点，主要是为了物品的掉落进行了安全
 *       的管理
**/
#ifndef SCENE_MANAGER_DROPPOSITION_H_
#define SCENE_MANAGER_DROPPOSITION_H_

#include "scene/manager/config.h"

namespace scene {

namespace manager {

class PF_PLUGIN_API DropPosition {

 public:
   DropPosition();
   ~DropPosition();

 public:
   bool init(uint32_t mapwidth, uint32_t mapheight);
   void cleanup();

 public:
   void setscene(scene::Scene *scene);
   //x, z即x, y 
   bool candrop(float x, float z);
   void setposition(float x, float z);
   void clearposition(float x, float z);
   void set_dropdistance(float distance);

 protected:
   uint8_t *dropposition_;
   uint32_t z_dropnumber_;
   uint32_t x_dropnumber_;
   float dropdistance_;
   scene::Scene *scene_;

};

}; //namespace manager

}; //namespace scene

#endif //SCENE_MANAGER_DROPPOSITION_H_
