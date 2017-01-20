#include "scene/scaner.h"

namespace scene {

Scaner::Scaner() {
  __ENTER_FUNCTION
    scene_ = NULL;
    gridid_ = ID_INVALID;
    gridradius_ = 0;
    scanhuman_ = false;
  __LEAVE_FUNCTION
}

Scaner::~Scaner() {
  //do nothing
}

bool Scaner::init(scaner_init_t *initdata) {
  __ENTER_FUNCTION
    if (NULL == initdata) return false;
    scene_ = initdata->scene;
    gridid_ = initdata->gridid;
    gridradius_ = initdata->gridradius;
    scanhuman_ = initdata->scanhuman;
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Scaner::on_beforescan() {
  return true;
}

bool Scaner::on_needscan(gridid_t gridid) {
  USE_PARAM(gridid);
  return true;
}

uint32_t Scaner::on_findobject(structure::Object *object) {
  USE_PARAM(object);
  return 0;
}

void Scaner::on_afterscan() {
  //do nothing
}

Scene *Scaner::getscene() {
  return scene_;
}

} //namespace scene
