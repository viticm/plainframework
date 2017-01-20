#include "structure/define.h"
#include "scene/manager/pkregion.h"

namespace scene {

namespace manager {

PKRegion::PKRegion() {
  __ENTER_FUNCTION
    count_ = 0;
    scene_ = NULL;
    config_ = NULL;
  __LEAVE_FUNCTION
}

PKRegion::~PKRegion() {
  //do nothing
}

bool PKRegion::init(const char *filename, sceneid_t sceneid) {
  USE_PARAM(filename);
  USE_PARAM(sceneid);
  return true;
}

void PKRegion::destroy() {
  __ENTER_FUNCTION
    SAFE_DELETE(config_);
    count_ = 0;
  __LEAVE_FUNCTION
}

bool PKRegion::insafe(float x, 
                      float z, 
                      campid_t campid, 
                      campid_t &region_campid) {
  __ENTER_FUNCTION
    using namespace structure;
    USE_PARAM(campid);
    uint32_t i;
    bool result = false;
    region_campid = CAMP_INVALID;
    for (i = 0; i < count_; ++i) {
      pkregion_t *pkregion = &config_->pkregion[i];
      if (!pkregion) continue;
      if (pkregion->type != 0 || pkregion->type != 1) continue;
      /**
      if (campid != CAMP_INVALID && pkregion->base_campid != CAMP_INVALID) {
        human_campdata_t campdata1;
        human_campdata_t campdata2;
        campdata1.id = campid;
        campdata2.id = pkregion->base_campid;
        
      }
      **/
      if (0 == pkregion->type) {
        if (pkregion->iscontain(x, z)) {
          region_campid = pkregion->base_campid;
          result = true;
          break;
        }
      }
    } //for
    return result;
  __LEAVE_FUNCTION
    return false;
}

int32_t PKRegion::get_duel_regionid(float x, float z) const {
  __ENTER_FUNCTION
    int32_t result = -1;
    for (int32_t i = 0; i < static_cast<int32_t>(count_); ++i) {
      pkregion_t *pkregion = &config_->pkregion[i];
      if (pkregion && 2 == pkregion->type) {
        if (pkregion->iscontain(x, z)) {
          result = i;
          break;
        }
      }
    }
    return result;
  __LEAVE_FUNCTION
    return -1;
}

} //namespace manager

} //namespace scene
