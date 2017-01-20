#include "scene/grid.h"

namespace scene {
   
Grid::Grid() {
  cleanup();
}

Grid::~Grid() {
  cleanup();
}

void Grid::cleanup() {
  __ENTER_FUNCTION
    objectlist_.cleanup();
    humanlist_.cleanup();
    id_ = ID_INVALID;
    regioncount_ = 0;
    memset((void *)regions_, 0, sizeof(regions_));
  __LEAVE_FUNCTION
}

void Grid::setid(gridid_t id) {
  id_ = id;
}

gridid_t Grid::getid() const {
  return id_;
}

structure::ObjectList *Grid::get_objectlist() {
  return &objectlist_;
}

structure::ObjectList *Grid::get_humanlist() {
  return &humanlist_;
}

bool Grid::on_objectenter(structure::Object *object) {
  USE_PARAM(object);
  return true;
}

bool Grid::on_objectleave(structure::Object *object) {
  USE_PARAM(object);
  return true;
}

void Grid::addregion(const region_t *region) {
  __ENTER_FUNCTION
    if (regioncount_ > SCENE_REGION_INZONE_MAX) return;
    regions_[regioncount_++] = region;
  __LEAVE_FUNCTION
}

const region_t *Grid::get_currentregion(const structure::position_t *position) {
  __ENTER_FUNCTION
    float x, z;
    x = position->x;
    z = position->z;
    const region_t *region = NULL;
    for (int32_t i = 0; i < regioncount_; ++i) {
      if (regions_[i] != NULL && regions_[i]->iscontain(x, z)) {
        region = regions_[i];
        break;
      } 
    }
    return region;
  __LEAVE_FUNCTION
    return NULL;
}

uint8_t Grid::get_regioncount() const {
  return regioncount_;
}
    
} //namespace scene
