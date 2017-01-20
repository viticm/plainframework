#include "scene/map.h"
#include "scene/pathfinder.h"

namespace scene {

Map::Map() {
  __ENTER_FUNCTION
    x_ = 0;
    z_ = 0;
    pathfinder_ = NULL;
  __LEAVE_FUNCTION
}

Map::~Map() {
  __ENTER_FUNCTION
    SAFE_DELETE(pathfinder_);
  __LEAVE_FUNCTION
}

bool Map::load(const char *filename) {
  __ENTER_FUNCTION
    SAFE_DELETE(pathfinder_);
    pathfinder_ = new PathFinder(this, filename, x_, z_);
    Assert(pathfinder_);
    return true;
  __LEAVE_FUNCTION
    return false;
}

void Map::cleanup() {
  __ENTER_FUNCTION
    SAFE_DELETE(pathfinder_);
  __LEAVE_FUNCTION
}

uint32_t Map::getx() const {
  return x_;
}

uint32_t Map::getz() const {
  return z_;
}

void Map::verifyposition(structure::position_t *posistion) {
  __ENTER_FUNCTION
    posistion->x = posistion->x < 0 ? 0 : posistion->x;
    posistion->z = posistion->z < 0 ? 0 : posistion->z;
    posistion->x = posistion->x >= static_cast<float>(x_) ? 
                   static_cast<float>(x_) - 0.1f : 
                   posistion->x;
    posistion->z = posistion->z >= static_cast<float>(z_) ?
                   static_cast<float>(z_) - 0.1f :
                   posistion->z;
  __LEAVE_FUNCTION
}

PathFinder *Map::get_pathfinder() {
  return pathfinder_;
}

bool Map::cango(const structure::position_t &posistion) {
  __ENTER_FUNCTION
    bool result = false;
    if (pathfinder_) result = pathfinder_->cango(posistion);
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Map::find_emptyposition(structure::position_t &posistion) {
  __ENTER_FUNCTION
    if (cango(posistion)) return true;
    bool result = false;
    const uint8_t kFindSizeMax = 24;
    for (uint8_t i = 0; i < kFindSizeMax; ++i) {
      structure::position_t temp_position;
      temp_position.x = posistion.x + i;
      temp_position.z = posistion.z;
      verifyposition(&temp_position);
      if (cango(temp_position)) {
        posistion = temp_position;
        result = true;
        break;
      }
      temp_position.x = posistion.x;
      temp_position.z = posistion.z + i;
      verifyposition(&temp_position);
      if (cango(temp_position)) {
        posistion = temp_position;
        result = true;
        break;
      }
      temp_position.x = posistion.x - i;
      temp_position.z = posistion.z;
      verifyposition(&temp_position);
      if (cango(temp_position)) {
        posistion = temp_position;
        result = true;
        break;
      }
      temp_position.x = posistion.x;
      temp_position.z = posistion.z - i;
      verifyposition(&temp_position);
      if (cango(temp_position)) {
        posistion = temp_position;
        result = true;
        break;
      }
      temp_position.x = posistion.x + i;
      temp_position.z = posistion.z + i;
      verifyposition(&temp_position);
      if (cango(temp_position)) {
        posistion = temp_position;
        result = true;
        break;
      }
      temp_position.x = posistion.x - i;
      temp_position.z = posistion.z - i;
      verifyposition(&temp_position);
      if (cango(temp_position)) {
        posistion = temp_position;
        result = true;
        break;
      }
      temp_position.x = posistion.x + i;
      temp_position.z = posistion.z - i;
      verifyposition(&temp_position);
      if (cango(temp_position)) {
        posistion = temp_position;
        result = true;
        break;
      }
      temp_position.x = posistion.x - i;
      temp_position.z = posistion.z + i;
      verifyposition(&temp_position);
      if (cango(temp_position)) {
        posistion = temp_position;
        result = true;
        break;
      }
    }
    return result;
  __LEAVE_FUNCTION
    return false;
}

} //namespace scene
