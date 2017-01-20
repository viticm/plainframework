#include "scene/manager/region.h"

namespace scene {

namespace manager {

Region::Region() {
  __ENTER_FUNCTION
    count_ = 0;
    regions_ = NULL;
    scene_ = NULL;
  __LEAVE_FUNCTION
}

Region::~Region() {
  //do nothing
}

bool Region::init(const char *filename) {
  USE_PARAM(filename);
  return true;
}

void Region::setscene(scene::Scene *scene) {
  scene_ = scene;
}

void Region::destroy() {
  count_ = 0;
}

uint32_t Region::get_gridid(float x, float z) {
  __ENTER_FUNCTION
    uint32_t i;
    uint32_t result = UINT_MAX;
    for (i = 0; i < count_; ++i) {
      region_t *region = &regions_[i];
      if (region->iscontain(x, z)) {
        result = i;
        break;
      }
    }
    return result;
  __LEAVE_FUNCTION
    return UINT_MAX;
}

rect_t *Region::get_gridrect(gridid_t gridid) {
  __ENTER_FUNCTION
    rect_t *rect = NULL;
    if (static_cast<uint32_t>(gridid) < count_) {
      region_t *region = &regions_[gridid];
      rect = &region->rect;
    }
    return rect;
  __LEAVE_FUNCTION
    return NULL;
}

} //namespace manager

} //namespace scene
