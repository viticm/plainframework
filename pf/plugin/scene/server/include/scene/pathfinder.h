/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id pathfinder.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm@126.com>/<viticm.ti@gmail.com>
 * @date 2015/03/18 11:28
 * @uses the map path finder class
 *       cn: 地图寻路器，主要用于场景寻路，同时支持2D/3D寻路
 *           完全从天龙八部的寻路拿过来，使用的是A*算法
**/
#ifndef SCENE_PATHFINDER_H_
#define SCENE_PATHFINDER_H_

#include "scene/config.h"
#include "scene/define.h"

#define SCENE_PATHFINDER_ROUTES_MAX (1024 * 2)
#define SCENE_PATHFINDER_DIRECTIONS 4

namespace scene {

class PF_PLUGIN_API PathFinder {

 public:
   PathFinder(Map *owner, const char *filename, uint32_t &x, uint32_t &z);
   virtual ~PathFinder();

 public:
   void reset();
   void reset(int32_t startz, int32_t endz);
   bool findpath(structure::position_t *startposition, 
                 structure::position_t *endposition,
                 structure::position_t *positionnode,
                 int32_t &nodenumber, 
                 bool line = false, 
                 int32_t routemax = SCENE_PATHFINDER_ROUTES_MAX);
   void packroute(structure::position_t *positionnode, int32_t &nodenumber);
   int32_t distance() const;
   bool cango(const structure::position_t &position);
   bool add_user_definebar(const structure::position_t &lt, const structure::position_t &rb);
   bool remove_user_definebar(const structure::position_t &lt, const structure::position_t &rb);
   int32_t getwidth() const;
   int32_t getheight() const;

 private:
   struct _D2XZLOOKUP {
     uint16_t costmultiplier;
     int16_t zx;
     uint8_t route;
   } DZX[SCENE_PATHFINDER_DIRECTIONS];
   struct AIROUTE {
     uint16_t count;
     uint16_t walkpoint;
     uint32_t startzx;
     uint32_t endzx;
     uint8_t route[SCENE_PATHFINDER_ROUTES_MAX];
   };
   struct _WORLD {
     uint8_t basestate : 3;
     uint8_t runtimestate : 2;
     uint8_t route : 3;
   };
   struct _NODES {
     uint16_t f;
     uint16_t g;
     uint32_t zx;
   };
   struct _NAVMAP_HEAD {
     uint16_t width;
     uint16_t height;
   };
   enum {
     kEmptyNode = 0,
     kMinNode = 1,
     kNoRoute = SCENE_PATHFINDER_DIRECTIONS,
     kHeapLeafsMax = SCENE_PATHFINDER_ROUTES_MAX,
   };
   enum {
     kUnknown = 0,
     kImpassable = 1,
     kOpen = 2,
     kClosed = 3,
     kTempImpassable = 4,
     kTempImpassableCross = 5,
   };
   enum {
     kRootHeap = 1,
   };

 private:
#ifdef _DEBUG
   char filename_[FILENAME_MAX];
   Map *owner_;
#endif
   int32_t width_;
   int32_t height_;
   float gridsize_;
   float inv_gridsize_;
   float lefttop_x_;
   float righttop_z_;
   _WORLD *world_;
   _NODES *nodes_;
   uint16_t last_heapleaf_;
   uint16_t heap_[kHeapLeafsMax];
   uint16_t best_fnode_;
   uint16_t freenode_;
   int32_t nodemax_;
   uint32_t startzx_, endzx_;
   uint16_t startx_, startz_, endx_, endz_;
   float fstartx_, fstartz_, fendx_, fendz_;
   int32_t calltimes_;
   int32_t gridnumber_;
   int32_t grids_[SCENE_PATHFINDER_ROUTES_MAX];
   structure::position_t first_cango_position_;
   int32_t distance_;
   int32_t currentstep_;

 private:
   inline uint16_t distance(const uint32_t zx);
   inline uint16_t realdistance(const uint32_t zxFrist, const uint32_t zxSecond);
   inline int32_t left(int32_t k);
   inline int32_t right(int32_t k);
   inline int32_t parent(int32_t k);
   inline bool nodeempty_up(int32_t k);
   inline bool nodeempty_down(int32_t k);
   inline void swapheap(const int32_t k1, const int32_t k2);
   inline bool isimpassable(const uint8_t state);
   inline void copy_impassablestate(_WORLD *targetstate);

 private:
   void insertnode_toheap(uint16_t node);
   void remove_root_fromheap();
   void read_navmap(const char *filename, uint32_t &x, uint32_t &z);
   void init_fourdirections();
   bool is_straightline(float startx, 
                        float starty, 
                        float endx, 
                        float endy, 
                        bool edit);
   void edit_astar_path(float startx, 
                        float starty, 
                        structure::position_t *nodeposition,
                        int32_t &nodenumber);
   void edit_astar_pathopt(float startx, 
                           float starty, 
                           structure::position_t *nodeposition,
                           int32_t &nodenumber);
   

};

}; //namespace scene

#endif //SCENE_PATHFINDER_H_
