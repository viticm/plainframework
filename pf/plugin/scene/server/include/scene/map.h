/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id map.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm@126.com>/<viticm.ti@gmail.com>
 * @date 2015/03/18 11:00
 * @uses scene map class
 *       cn: 场景地图类，主要负责场景的地图数据操作，主要用于寻路
**/
#ifndef SCENE_MAP_H_
#define SCENE_MAP_H_

#include "scene/config.h"
#include "scene/define.h"

/**
 *            (0, z)   (x, z)
 *  y  z      ----------     
 *  | /       |        |
 *  |/        |        |
 *  +------x  |        |
 *            ----------
 *            (0, 0)   (x, 0)
 *  在2D地图中y是没有意义的，或默认为0          
**/
namespace scene {

class PF_PLUGIN_API Map {

 public:
   Map();
   ~Map();

 public:
   bool load(const char *filename);
   void cleanup();
   uint32_t getx() const;
   uint32_t getz() const;

 public:
   void verifyposition(structure::position_t *posistion);
   PathFinder *get_pathfinder();
   bool cango(const structure::position_t &posistion);
   bool find_emptyposition(structure::position_t &posistion);

 private:
   uint32_t x_;
   uint32_t z_;
   PathFinder *pathfinder_;

};

}; //namespace scene

#endif //SCENE_MAP_H_
