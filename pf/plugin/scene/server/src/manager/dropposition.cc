#include "pf/base/log.h"
#include "scene/scene.h"
#include "scene/map.h"
#include "scene/manager/dropposition.h"

namespace scene {

const uint16_t kDropPositionSizeMax = 512;

namespace manager {

  DropPosition::DropPosition() {
    __ENTER_FUNCTION
    dropposition_ = NULL;
    z_dropnumber_ = x_dropnumber_ = 0;
    dropdistance_ = 2.0f;
    scene_ = NULL;
    __LEAVE_FUNCTION
  }

  DropPosition::~DropPosition() {
    __ENTER_FUNCTION
      cleanup();
    __LEAVE_FUNCTION
  }

  bool DropPosition::init(uint32_t mapwidth, uint32_t mapheight) {
    __ENTER_FUNCTION
      if (mapwidth > kDropPositionSizeMax || mapheight > kDropPositionSizeMax)
        return false;
    if (0 == mapwidth || 0 == mapheight) {
      SLOW_ERRORLOG(SCENE_MODULENAME,
        "[scene.manager] (DropPosition::init) error"
        " mapwidth: %d, mapheight: %d",
        mapwidth,
        mapheight);
      return false;
    }
    while ((z_dropnumber_ + 1) * dropdistance_ < mapheight) ++z_dropnumber_;
    while ((x_dropnumber_ + 1) * dropdistance_ < mapwidth) ++x_dropnumber_;
    dropposition_ = new uint8_t[x_dropnumber_ * z_dropnumber_];
    Assert(dropposition_);
    memset(dropposition_, 0, sizeof(uint8_t) * x_dropnumber_ * z_dropnumber_);
    structure::position_t position;
    for (uint32_t i = 0; i < z_dropnumber_; ++i) {
      for (uint32_t j = 0; j < x_dropnumber_; ++j) {
        position.x = (j + 1) * dropdistance_;
        position.z = (i + 1) * dropdistance_;
        if (!scene_->getmap()->cango(position))
          dropposition_[j + i * x_dropnumber_] = static_cast<uint8_t>(-1);
      }
    }
    return true;
    __LEAVE_FUNCTION
      return false;
  }

  void DropPosition::cleanup() {
    __ENTER_FUNCTION
      if (NULL == dropposition_ || 0 == z_dropnumber_ || 0 == x_dropnumber_)
        return;
    SAFE_DELETE_ARRAY(dropposition_);
    x_dropnumber_ = 0;
    z_dropnumber_ = 0;
    __LEAVE_FUNCTION
  }

  void DropPosition::setscene(scene::Scene *scene) {
    scene_ = scene;
  }

  bool DropPosition::candrop(float x, float z) {
    __ENTER_FUNCTION
      if (NULL == scene_) return false;
    if (NULL == dropposition_ || 0 == z_dropnumber_ || 0 == x_dropnumber_)
      return false;
    //x check
    if (x < dropdistance_ || x > x_dropnumber_ * dropdistance_) return false;
    //z check
    if (z < dropdistance_ || z > z_dropnumber_ * dropdistance_) return false;
    structure::position_t position;
    position.x = x;
    position.z = z;
    if (!scene_->getmap()->cango(position)) return false;
    //计算掉落间网格的索引
    uint32_t _x = static_cast<uint32_t>(x / dropdistance_);
    uint32_t _z = static_cast<uint32_t>(z / dropdistance_);
    bool result = false;
    if (_x <= x_dropnumber_ && _z <= z_dropnumber_) {
      if (-1 == dropposition_[(_x - 1) + x_dropnumber_ * (_z - 1)])
        return false;
      result = !(dropposition_[(_x - 1) + x_dropnumber_ * (_z - 1)]);
    }
    return result;
    __LEAVE_FUNCTION
      return false;
  }

  void DropPosition::setposition(float x, float z) {
    __ENTER_FUNCTION
      if (NULL == dropposition_ || 0 == z_dropnumber_ || 0 == x_dropnumber_)
        return;
    //x check
    if (x < dropdistance_ || x > x_dropnumber_ * dropdistance_) return;
    //z check
    if (z < dropdistance_ || z > z_dropnumber_ * dropdistance_) return;
    uint32_t _x = static_cast<uint32_t>(x / dropdistance_);
    uint32_t _z = static_cast<uint32_t>(z / dropdistance_);
    if (_x <= x_dropnumber_ && _z <= z_dropnumber_) {
      dropposition_[(_x - 1) + x_dropnumber_ * (_z - 1)] = 1;
    }
    __LEAVE_FUNCTION
  }

  void DropPosition::clearposition(float x, float z) {
    __ENTER_FUNCTION
      if (NULL == dropposition_ || 0 == z_dropnumber_ || 0 == x_dropnumber_)
        return;
    //x check
    if (x < dropdistance_ || x > x_dropnumber_ * dropdistance_) return;
    //z check
    if (z < dropdistance_ || z > z_dropnumber_ * dropdistance_) return;
    uint32_t _x = static_cast<uint32_t>(x / dropdistance_);
    uint32_t _z = static_cast<uint32_t>(z / dropdistance_);
    if (_x <= x_dropnumber_ && _z <= z_dropnumber_) {
      dropposition_[(_x - 1) + x_dropnumber_ * (_z - 1)] = 0;
    }
    __LEAVE_FUNCTION
  }

} //namespace manager

} //namespace scene
