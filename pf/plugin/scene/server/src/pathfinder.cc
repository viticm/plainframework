#include "pf/base/log.h"
#include "scene/pathfinder.h"
#include "scene/map.h"

namespace scene {

#define CAN_GO_EDGE

#define RECURETIMES 16
#define OFFSETZ 128

PathFinder::PathFinder(Map *owner, 
                       const char *filename, 
                       uint32_t &mx, 
                       uint32_t &mz) {
  __ENTER_FUNCTION
#ifdef _DEBUG
    strcpy(filename_, filename);
    owner_ = owner;
#else
    USE_PARAM(owner);
#endif
    read_navmap(filename, mx, mz);
    init_fourdirections();
  __LEAVE_FUNCTION
}

void PathFinder::read_navmap(const char *filename, uint32_t &mx, uint32_t &mz) {
  __ENTER_FUNCTION 
    FILE *fp = fopen(filename, "rb");
    if (NULL == fp) {
      char szDesc[64] = {0};
      snprintf(szDesc, 64, "PathFinder::read_navmap[%s] Fail.", filename);
      AssertEx(0, szDesc);
      return;
    }
    _NAVMAP_HEAD  head;
    fread(&head, sizeof(_NAVMAP_HEAD), 1, fp);

    width_ = head.width;
    height_ = head.height;
    gridsize_ = 0.5;
    inv_gridsize_ = 1 / gridsize_;

    mx = (uint32_t) (width_ * gridsize_);
    mz = (uint32_t) (height_ * gridsize_);

    lefttop_x_ = 0;
    righttop_z_ = 0;

    nodemax_ = width_ * height_;

    world_ = new _WORLD[nodemax_];
    nodes_ = new _NODES[nodemax_ + 1];

    int32_t size = sizeof(struct _NAVMAP_HEAD);
    for (int32_t j = 0; j < height_; j++) {
      for (int32_t i = 0; i < width_; i++) {
        int32_t  info;
        fread(&info, size, 1, fp);
        _WORLD  *pWorld = world_ + j * width_ + i;
        pWorld->basestate = info;
      }
    }
    nodes_[0].zx = 0;
    nodes_[0].f = 0;
    nodes_[0].g = 0;
    currentstep_ = 0;
    fclose(fp);
  __LEAVE_FUNCTION
}

void PathFinder::reset() {
  __ENTER_FUNCTION
    for (int32_t j = 0; j < height_; j++) {
      for (int32_t i = 0; i < width_; i++) {
        _WORLD  *pWorld = world_ + j * width_ + i;
        if (pWorld->basestate == kTempImpassable)
          pWorld->basestate = kUnknown;
        else if (pWorld->basestate == kTempImpassableCross)
          pWorld->basestate = kImpassable;
      }
    }
  __LEAVE_FUNCTION
}

void PathFinder::init_fourdirections() {
  __ENTER_FUNCTION 
    int32_t n;
    for (n = 0; n < SCENE_PATHFINDER_DIRECTIONS; n++) {
      DZX[n].costmultiplier = 10;
    }

    DZX[0].zx = static_cast<int16_t>(-width_);
    DZX[0].route = 2;

    DZX[1].zx = 1;
    DZX[1].route = 3;

    DZX[2].zx = static_cast<int16_t>(width_);
    DZX[2].route = 0;

    DZX[3].zx = -1;
    DZX[3].route = 1;
  __LEAVE_FUNCTION
}


PathFinder::~PathFinder() {
  __ENTER_FUNCTION  
    SAFE_DELETE_ARRAY(world_);
    SAFE_DELETE_ARRAY(nodes_);
  __LEAVE_FUNCTION
}

void PathFinder::reset(int32_t startz, int32_t endz) {
  __ENTER_FUNCTION  
    Assert(startz >= 0 && endz < height_);
    for (register int32_t index = startz * width_; 
         index < endz * width_; 
         ++index) {
      copy_impassablestate(&world_[index]);
    }
    best_fnode_ = 1;
    nodes_[best_fnode_].zx = startzx_;
    nodes_[best_fnode_].g = 0;
    nodes_[best_fnode_].f = nodes_[best_fnode_].g + distance(startzx_);
    world_[startzx_].route = kNoRoute;
    freenode_ = 1;
    heap_[0] = kEmptyNode;
    last_heapleaf_ = 1;
    heap_[last_heapleaf_] = best_fnode_;
  __LEAVE_FUNCTION
}

void PathFinder::remove_root_fromheap() {
  __ENTER_FUNCTION  
    heap_[kRootHeap] = heap_[last_heapleaf_--];
    int32_t k = kRootHeap;
    while (nodeempty_down(k)) {
      int32_t leftk = left(k);
      int32_t rightk = right(k);
      int32_t bestk;

      if (nodeempty_down(leftk) && nodeempty_down(rightk)) {
        if (nodes_[heap_[leftk]].f < nodes_[heap_[rightk]].f)
          bestk = leftk;
        else
          bestk = rightk;
      }
      else if (nodeempty_down(leftk))
        bestk = leftk;
      else
        break;

      if (nodes_[heap_[bestk]].f < nodes_[heap_[k]].f) {
        swapheap(k, bestk);
        k = bestk;
      }
      else
        break;
    }
  __LEAVE_FUNCTION
}

void PathFinder::insertnode_toheap(uint16_t node) {
  __ENTER_FUNCTION 
    if (last_heapleaf_ < kHeapLeafsMax) last_heapleaf_++;
    heap_[last_heapleaf_] = node;
    int32_t  k = last_heapleaf_;
    while (nodeempty_up(k)) {
      int32_t  parentk = parent(k);
      if (nodeempty_up(parentk)) {
        if (nodes_[heap_[k]].f < nodes_[heap_[parentk]].f) {
          swapheap(k, parentk);
          k = parentk;
        }
        else
          break;
      }
      else
        break;
    }
  __LEAVE_FUNCTION
}


inline int32_t PathFinder::left(int32_t k) {
  __ENTER_FUNCTION 
    return k << 1;
  __LEAVE_FUNCTION
    return 0;
}

inline int32_t PathFinder::right(int32_t k) {
  __ENTER_FUNCTION 
    return (k << 1) + 1;
  __LEAVE_FUNCTION
    return 0;
}

inline int32_t PathFinder::parent(int32_t k) {
  __ENTER_FUNCTION 
    return (k >> 1);
  __LEAVE_FUNCTION
    return 0;
}

inline bool PathFinder::nodeempty_up(int32_t k) {
  __ENTER_FUNCTION 
    return k != 0;
  __LEAVE_FUNCTION
    return false;
}

inline bool PathFinder::nodeempty_down(int32_t k) {
  __ENTER_FUNCTION 
    return k <= last_heapleaf_;
  __LEAVE_FUNCTION
    return false;
}

inline void PathFinder::swapheap(const int32_t k1, const int32_t k2) {
  __ENTER_FUNCTION 
    uint16_t tmp = heap_[k1];
    heap_[k1] = heap_[k2];
    heap_[k2] = tmp;
  __LEAVE_FUNCTION
}

bool PathFinder::findpath(structure::position_t *startposition,
                          structure::position_t *endposition,
                          structure::position_t *positionnode,
                          int32_t &nodenumber,
                          bool line,
                          int32_t routemax) {

  __ENTER_FUNCTION
#ifdef _DEBUG
    if (!(startposition->x >= 0
      &&  startposition->x < owner_->getx()
      &&  startposition->z >= 0
      &&  startposition->z < owner_->getz()) || !(
      endposition->x >= 0
      &&  endposition->x < owner_->getx()
      &&  endposition->z >= 0
      &&  endposition->z < owner_->getz())) {
      FAST_ERRORLOG(kErrorLogFile, "PathFinder::findpath error");
      return false;
    }
#endif
    fstartx_ = startposition->x;
    fstartz_ = startposition->z;
    fendx_ = endposition->x;
    fendz_ = endposition->z;

    startx_ = static_cast<uint16_t>((fstartx_ - lefttop_x_) * inv_gridsize_);
    startz_ = static_cast<uint16_t>((fstartz_ - righttop_z_) * inv_gridsize_);

    endx_ = static_cast<uint16_t>((fendx_ - lefttop_x_) * inv_gridsize_);
    endz_ = static_cast<uint16_t>((fendz_ - righttop_z_) * inv_gridsize_);

    startzx_ = startz_ * width_ + startx_;
    endzx_ = endz_ * width_ + endx_;

    nodenumber = 0;

    distance_ = 0;

    if (line) {
      if (!isimpassable(world_[startzx_].basestate)) {
        if (is_straightline(fstartx_, fstartz_, fendx_, fendz_, false)) {
          positionnode[nodenumber++] = structure::position_t(fendx_, fendz_);
          distance_ = realdistance(startzx_, endzx_);
          return true;
        } else {
          positionnode[nodenumber++] = first_cango_position_;
          distance_ = realdistance(
              startzx_,
              static_cast<uint16_t>(
                (first_cango_position_.z - righttop_z_) * inv_gridsize_) * 
                width_ +
              static_cast<uint16_t>((first_cango_position_.x - lefttop_x_) * 
                inv_gridsize_));
          return true;
        }
      } else {
        return false;
      }
    }
    if (isimpassable(world_[startzx_].basestate) || 
        isimpassable(world_[endzx_].basestate)) {
      return false;
    } else {
      int32_t  deltax = abs(endx_ - startx_);
      int32_t  deltaz = abs(endz_ - startz_);
      if ((deltax + deltaz) > routemax) {
        distance_ = realdistance(
            startzx_,
            static_cast<uint16_t>(
              (first_cango_position_.z - righttop_z_) * inv_gridsize_) * width_ + 
            static_cast<uint16_t>(
              (first_cango_position_.x - lefttop_x_) * inv_gridsize_));
        return false;
      }

      if (is_straightline(fstartx_, fstartz_, fendx_, fendz_, false)) {
        positionnode[nodenumber++] = structure::position_t(fendx_, fendz_);
        distance_ = realdistance(startzx_, endzx_);
        return true;
      }
      int32_t minz = min(startz_, endz_);
      int32_t maxz = max(startz_, endz_);
      minz = max(minz - OFFSETZ, 0);
      maxz = min(maxz + OFFSETZ, height_ - 1);
      reset(minz, maxz);
      int32_t count = routemax;
      do {
        best_fnode_ = heap_[kRootHeap];
        _NODES  *pparent_node = nodes_ + best_fnode_;

        if (pparent_node->zx == endzx_) {
          packroute(positionnode, nodenumber);
          return true;
        }

        world_[pparent_node->zx].runtimestate = kClosed;
        remove_root_fromheap();

        for (uint8_t d = 0; d < SCENE_PATHFINDER_DIRECTIONS; d++) {
#ifdef CAN_GO_EDGE
          int32_t  zx = static_cast<int32_t>(pparent_node->zx);
          if ((zx % width_ == 0) && ((d == 3) || (d == 6) || (d == 7))) 
            continue;
          if ((zx % width_ == (width_ - 1)) && 
              ((d == 1) || (d == 4) || (d == 5))) continue;

          zx += DZX[d].zx;
          if (zx < 0 || zx >= width_ * height_) continue;

#else
          uint16_t  zx = pparent_node->zx + DZX[d].zx;
#endif
          _WORLD  *pworld = world_ + zx;
          if (pworld->runtimestate == kUnknown) {
            pworld->runtimestate = kOpen;
            pworld->route = d;
            ++freenode_;
            _NODES  *pfree_node = nodes_ + freenode_;
            pfree_node->zx = zx;
            pfree_node->g = pparent_node->g + DZX[d].costmultiplier;
            pfree_node->f = pfree_node->g + distance(zx);

            insertnode_toheap(freenode_);
          }
        }

        if (last_heapleaf_ <= 0) {
          positionnode[nodenumber++] = first_cango_position_;
          distance_ = realdistance(
              startzx_,
              static_cast<uint16_t>(
                (first_cango_position_.z - righttop_z_) * inv_gridsize_) * 
              width_ +
              static_cast<uint16_t>(
                (first_cango_position_.x - lefttop_x_) * inv_gridsize_));
          return true;
        }
      } while (--count > 0);
#ifdef _DEBUG
      count;
#endif
    }
  __LEAVE_FUNCTION 
    return false;
}

inline uint16_t PathFinder::distance(const uint32_t zx) {
  __ENTER_FUNCTION 
   uint16_t result = static_cast<uint16_t>(
       (abs(static_cast<int32_t>(zx & (width_ - 1)) - 
          static_cast<int32_t>(endx_)) + 
        abs(static_cast<int32_t>(zx / width_) - 
          static_cast<int32_t>(endz_))) * 10);
    return result;
  __LEAVE_FUNCTION
    return 0;
}

inline uint16_t PathFinder::realdistance(const uint32_t zxFirst, 
                                         const uint32_t zxSecond) {
  __ENTER_FUNCTION 
    uint16_t result = 
      static_cast<uint16_t>((abs((int32_t) (zxFirst & (width_ - 1)) - 
        static_cast<int32_t>(zxSecond & (width_ - 1))) + 
        abs(
          static_cast<int32_t>(zxFirst / width_) - 
          static_cast<int32_t>(zxSecond / width_))));
      return result;
  __LEAVE_FUNCTION
    return 0;
}

void PathFinder::packroute(structure::position_t *positionnode, int32_t &nodenumber) {
  __ENTER_FUNCTION 
    AIROUTE airoute;
    memset(airoute.route, 0, SCENE_PATHFINDER_ROUTES_MAX);
    uint32_t zx = endzx_;
    //int32_t start = SCENE_PATHFINDER_ROUTES_MAX - 1;
    uint8_t route = kNoRoute;
    gridnumber_ = 0;
    structure::position_t  firstPos;
    grids_[gridnumber_++] = zx;
    distance_ = 0;
    while (zx != startzx_) {
      route = world_[zx].route;
      zx += DZX[DZX[route].route].zx;
      int32_t    x = zx % width_;
      int32_t    z = zx / width_;
      structure::position_t  goPos;
      goPos.x = lefttop_x_ + x * gridsize_ + gridsize_ / 2;
      goPos.z = righttop_z_ + z * gridsize_ + gridsize_ / 2;

      if (is_straightline(fstartx_, fstartz_, goPos.x, goPos.z, true)) {
        firstPos.x = lefttop_x_ + x * gridsize_ + gridsize_ / 2;
        firstPos.z = righttop_z_ + z * gridsize_ + gridsize_ / 2;

        positionnode[nodenumber++] = firstPos;
        distance_ += realdistance(zx, grids_[gridnumber_ - 1]);
        break;
      } else {
        grids_[gridnumber_++] = zx;
        distance_ += realdistance(zx, grids_[gridnumber_ - 1]);
      }
    }

    if (gridnumber_) {
      calltimes_ = 0;
      currentstep_ = gridnumber_;
      edit_astar_pathopt(firstPos.x, firstPos.z, positionnode, nodenumber);
    }
  __LEAVE_FUNCTION
}

int32_t PathFinder::distance() const {
  return distance_;
}

bool PathFinder::is_straightline(float startx,
                                 float starty,
                                 float endx,
                                 float endz,
                                 bool edit) {
  __ENTER_FUNCTION 
    int32_t intersecttime = 0;
    structure::position_t  startposition(startx, starty);
    structure::position_t  endposition(endx, endz);
    int32_t    xinc1, yinc1;
    int32_t    x1, y1, x2, y2;
    x1 = static_cast<int32_t>((startposition.x - lefttop_x_) * inv_gridsize_);
    y1 = static_cast<int32_t>((startposition.z - righttop_z_) * inv_gridsize_);
    x2 = static_cast<int32_t>((endposition.x - lefttop_x_) * inv_gridsize_);
    y2 = static_cast<int32_t>((endposition.z - righttop_z_) * inv_gridsize_);
    float  deltax, deltay;
    deltax = (endposition.x - startposition.x) * inv_gridsize_;
    deltay = (endposition.z - startposition.z) * inv_gridsize_;
    first_cango_position_ = startposition;
    if (fabs(deltax) >= fabs(deltay)) {
      float  slerp = deltay / deltax;
      if (x2 >= x1) {
        xinc1 = 1;
        for (int32_t x = x1 + xinc1; x <= x2; x += xinc1) {
          int32_t y = static_cast<int32_t>(
              ((lefttop_x_ + x * gridsize_ - startposition.x) * 
               slerp + startposition.z - righttop_z_) *
              inv_gridsize_);
          int32_t index = x + y * width_;
          if (isimpassable(world_[index].basestate) && 
              (edit ? intersecttime++ : 1))
            return false;
          first_cango_position_ = structure::position_t(
              lefttop_x_ + x * gridsize_ + gridsize_ / 2,
              righttop_z_ + y * gridsize_ + gridsize_ / 2);
          index = x - xinc1 + y * width_;
          if (isimpassable(world_[index].basestate) && 
              (edit ? intersecttime++ : 1))
            return false;
          first_cango_position_ = structure::position_t(
              lefttop_x_ + (x - xinc1) * gridsize_ + gridsize_ / 2,
              righttop_z_ + y * gridsize_ + gridsize_ / 2);
        }
      }else {
        xinc1 = -1;
        for (int32_t x = x1; x >= x2 - xinc1; x += xinc1) {
          int32_t y = static_cast<int32_t>(
              (startposition.z +
                slerp *
                ((lefttop_x_ + x * gridsize_) - startposition.x) -
                righttop_z_) * inv_gridsize_);
          int32_t  index = x + y * width_;
          if (isimpassable(world_[x + y * width_].basestate) && 
              (edit ? intersecttime++ : 1))
            return false;

          first_cango_position_ = structure::position_t(
              lefttop_x_ + x * gridsize_ + gridsize_ / 2,
              righttop_z_ + y * gridsize_ + gridsize_ / 2);

          index = x + xinc1 + y * width_;

          if (isimpassable(world_[index].basestate) && 
              (edit ? intersecttime++ : 1))
            return false;

          first_cango_position_ = structure::position_t(
              lefttop_x_ + (x + xinc1) * gridsize_ + gridsize_ / 2,
              righttop_z_ + y * gridsize_ + gridsize_ / 2);
        }
      }
    } else {
      float  slerpInv = deltax / deltay;
      if (y2 >= y1) {
        yinc1 = 1;
        for (int32_t y = y1 + yinc1; y <= y2; y += yinc1) {
          int32_t x = static_cast<int32_t>(
              (((righttop_z_ + y * gridsize_) - startposition.z) *
                slerpInv +
                startposition.x -
                lefttop_x_) * inv_gridsize_);
          int32_t  index = x + y * width_;
          if (isimpassable(world_[index].basestate) && 
              (edit ? intersecttime++ : 1))
            return false;

          first_cango_position_ = structure::position_t(
              lefttop_x_ + x * gridsize_ + gridsize_ / 2,
              righttop_z_ + y * gridsize_ + gridsize_ / 2);

          index = x + (y - yinc1) * width_;

          if (isimpassable(world_[index].basestate) && 
              (edit ? intersecttime++ : 1))
            return false;

          first_cango_position_ = structure::position_t(
              lefttop_x_ + x * gridsize_ + gridsize_ / 2,
              righttop_z_ + (y - yinc1) * gridsize_ + gridsize_ / 2);
        }
      } else {
        yinc1 = -1;
        for (int32_t y = y1; y >= y2 - yinc1; y += yinc1) {
          int32_t  x = (int32_t)
            (
              (
                startposition.x +
                slerpInv *
                ((righttop_z_ + y * gridsize_) - startposition.z) -
                lefttop_x_
              ) * inv_gridsize_
            );
          int32_t  index = x + y * width_;

          if (isimpassable(world_[index].basestate) && 
              (edit ? intersecttime++ : 1))
            return false;

          first_cango_position_ = structure::position_t(
              lefttop_x_ + x * gridsize_ + gridsize_ / 2,
              righttop_z_ + y * gridsize_ + gridsize_ / 2);

          index = x + (y + yinc1) * width_;

          if (isimpassable(world_[index].basestate) && 
              (edit ? intersecttime++ : 1))
            return false;

          first_cango_position_ = structure::position_t(
              lefttop_x_ + x * gridsize_ + gridsize_ / 2,
              righttop_z_ + (y + yinc1) * gridsize_ + gridsize_ / 2);
        }
      }
    }
    return true;
  __LEAVE_FUNCTION 
    return false;
}

void PathFinder::edit_astar_pathopt(float startpositionx, 
                                    float startpositiony, 
                                    structure::position_t *positionnode,
                                    int32_t &nodenumber) {
  __ENTER_FUNCTION 
    float tempStartX = startpositionx;
    float tempStartZ = startpositiony;
    int32_t tempCurrentStep = currentstep_;
    bool flag = false;

    while (tempCurrentStep != 0 && nodenumber <= RECURETIMES) {
      for (int32_t i = 0; i < currentstep_; i++) {
        int32_t    x = grids_[i] % width_;
        int32_t    y = grids_[i] / width_;
        structure::position_t  goPos;
        if (i != 0) {
          goPos.x = lefttop_x_ + x * gridsize_ + gridsize_ / 2;
          goPos.z = righttop_z_ + y * gridsize_ + gridsize_ / 2;
        } else {
          goPos = structure::position_t(fendx_, fendz_);
        }

        if (is_straightline(tempStartX, tempStartZ, goPos.x, goPos.z, true)) {
          positionnode[nodenumber++] = goPos;
          tempStartX = goPos.x;
          tempStartZ = goPos.z;
          currentstep_ = i;
          flag = true;
          break;
        }
      }
      if (flag) {
        tempCurrentStep = currentstep_;
        flag = false;
      } else{
        break;
      }
    }
  __LEAVE_FUNCTION
}

void PathFinder::edit_astar_path(float startpositionx, 
                                 float startpositiony, 
                                 structure::position_t *positionnode,
                                 int32_t &nodenumber) {
  __ENTER_FUNCTION 
    if (calltimes_++ >= RECURETIMES) return;
    int32_t  curGrid = 0;
    for (int32_t i = gridnumber_ - 1; i >= currentstep_; i--) {
      int32_t x = grids_[i] % width_;
      int32_t y = grids_[i] / width_;
      structure::position_t goPos;

      goPos.x = lefttop_x_ + x * gridsize_ + gridsize_ / 2;
      goPos.z = righttop_z_ + y * gridsize_ + gridsize_ / 2;

      if (is_straightline(
            startpositionx, startpositiony, goPos.x, goPos.z, true)) {
        currentstep_ = i;
        positionnode[nodenumber++] = goPos;
        curGrid = grids_[i];
        break;
      }
    }

    if (curGrid != grids_[gridnumber_ - 1]) {
      int32_t    x = curGrid % width_;
      int32_t    y = curGrid / width_;
      structure::position_t  startPos;
      startPos.x = lefttop_x_ + x * gridsize_ + gridsize_ / 2;
      startPos.z = righttop_z_ + y * gridsize_ + gridsize_ / 2;
      edit_astar_path(startPos.x, startPos.z, positionnode, nodenumber);
    }
  __LEAVE_FUNCTION
}

bool PathFinder::cango(const structure::position_t &pos) {
  __ENTER_FUNCTION 
    if (pos.x < 0.0f || pos.z < 0.0f) {
#ifdef _DEBUG
      SLOW_ERRORLOG(
        SCENE_MODULENAME,
        "ERROR PathFinder::cango(...) x = %f z = %f scene = %s",
        pos.x,
        pos.z,
        filename_);
#endif
      return false;
    }

    int32_t gridx = static_cast<uint16_t>((pos.x - lefttop_x_) * inv_gridsize_);
    int32_t gridz = static_cast<uint16_t>((pos.z - righttop_z_) * inv_gridsize_);

    if ((gridx >= 0 && gridx < width_) && (gridz >= 0 && gridz < height_)) {
      return !isimpassable(world_[gridz * width_ + gridx].basestate);
    }
    return false;
  __LEAVE_FUNCTION 
    return false;
}

bool PathFinder::isimpassable(const uint8_t state) {
  __ENTER_FUNCTION 
    if (kImpassable == state || kTempImpassable == state
        || kTempImpassableCross == state) 
        return true;
    return false;
  __LEAVE_FUNCTION 
    return false;
}

void PathFinder::copy_impassablestate(_WORLD *pWorld) {
  __ENTER_FUNCTION 
    if (pWorld == NULL) return;
    switch(pWorld->basestate) {
      case kTempImpassable:
      case kTempImpassableCross: {
        pWorld->runtimestate = kImpassable;
        break;
      }
      default:
        pWorld->runtimestate = pWorld->basestate;
    }
  __LEAVE_FUNCTION
}

bool PathFinder::add_user_definebar(const structure::position_t &lt,
                                    const structure::position_t &rb) {
  __ENTER_FUNCTION 
    int32_t ltx = (uint16_t) ((lt.x - lefttop_x_) * inv_gridsize_);
    int32_t ltz = (uint16_t) ((lt.z - righttop_z_) * inv_gridsize_);
    int32_t rbx = (uint16_t) ((rb.x - lefttop_x_) * inv_gridsize_);
    int32_t rbz = (uint16_t) ((rb.z - righttop_z_) * inv_gridsize_);

    if (ltx >= rbx) return false;
    if (ltz >= rbz) return false;
    if (rbx >= width_ || rbz >= height_) return false;

    int32_t  i;
    int32_t  j;

    for (i = ltx; i < rbx; i++) {
      for (j = ltz; j < rbz; j++) {
        int32_t  pos = j * width_ + i;
        switch(world_[pos].basestate) {
          case kUnknown: {
            world_[pos].basestate = kTempImpassable; 
            break;
          }
          case kImpassable: {
            world_[pos].basestate = kTempImpassableCross; 
            break;
          }
        }
      }
    } //for
    return true;
  __LEAVE_FUNCTION 
    return false;
}


bool PathFinder::remove_user_definebar(const structure::position_t &lt,
                                       const structure::position_t &rb) {
  __ENTER_FUNCTION 
    int32_t ltx = static_cast<uint16_t>((lt.x - lefttop_x_) * inv_gridsize_);
    int32_t ltz = static_cast<uint16_t>((lt.z - righttop_z_) * inv_gridsize_);
    int32_t rbx = static_cast<uint16_t>((rb.x - lefttop_x_) * inv_gridsize_);
    int32_t rbz = static_cast<uint16_t>((rb.z - righttop_z_) * inv_gridsize_);

    if (ltx >= rbx) return false;
    if (ltz >= rbz) return false;
    if (rbx >= width_ || rbz >= height_) return false;

    int32_t  i;
    int32_t  j;

    for (i = ltx; i < rbx; i++) {
      for (j = ltz; j < rbz; j++) {
        int32_t  pos = j * width_ + i;
        switch(world_[pos].basestate) {
          case kTempImpassable: {
            world_[pos].basestate = kUnknown; 
            break;
          }
          case kTempImpassableCross: {
            world_[pos].basestate = kImpassable; 
            break;
          }
        }
      }
    } //for
    return true;
  __LEAVE_FUNCTION 
    return false;
}

} //namespace scene
