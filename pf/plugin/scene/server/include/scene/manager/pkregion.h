/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id pkregion.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm@126.com>/<viticm.ti@gmail.com>
 * @date 2015/03/19 15:46
 * @uses scene pk region manager class
 *       cn: 安全区和非安全区管理
 *       这里就是非安全，可PK的区域管理器，在这个区域里玩家可以互相攻击
 *       属于特殊类型的区域，不属于地图的基本组成单位，有的游戏世界中就可能没有
 *       该区域
**/
#ifndef SCENE_MANAGER_PKREGION_H_
#define SCENE_MANAGER_PKREGION_H_

#include "scene/manager/config.h"

namespace scene {

namespace manager {

class PF_PLUGIN_API PKRegion {

 public:
   PKRegion();
   ~PKRegion();

 public:
   bool init(const char *filename, sceneid_t sceneid);
   void destroy();

 public:
   void setscene(scene::Scene *scene);
   virtual bool insafe(float x, 
                       float z, 
                       campid_t campid, 
                       campid_t &region_campid);
   int32_t get_duel_regionid(float x, float z) const;

 protected:
   uint32_t count_;
   scene::Scene *scene_;
   pkregion_config_t *config_;

};

}; //namespace manager

}; //namespace scene

#endif //SCENE_MANAGER_PKREGION_H_
