#include "scene/scene.h"
#include "scene/manager/fuben.h"

namespace scene {

namespace manager {

Fuben::Fuben() {
  __ENTER_FUNCTION
    for (uint16_t i = 0; i < SCENE_MAX; ++i) {
      scene_loadnode_[i].clear();
      scenes_[i] = NULL;
    }
    head_ = tail_ = 0;
    count_ = 0;
  __LEAVE_FUNCTION
}

Fuben::~Fuben() {
  //do nothing
}

bool Fuben::init() {
  return true;
}

sceneid_t Fuben::selectscene(fuben_select_t *select) {
  __ENTER_FUNCTION
    sceneid_t sceneid = ID_INVALID;
    if (NULL == select) return sceneid;
    scene::Scene *scene = NULL;
    pf_sys::lock_guard<pf_sys::ThreadLock> autolock(lock_);
    for (uint16_t i = 0; i < SCENE_MAX; ++i) {
      if (NULL == scenes_[i]) continue;
      if (kStatusSleep == scenes_[i]->getstatus()) {
        scenes_[i]->setstatus(kStatusSelect);
        scene = scenes_[i];
      } else {
        continue;
      }
      scene->load_ = select->sceneload;
      scene->initdata_.datatype = select->scene_datatype;
      scene->fuben_setdata(&select->fuben_data);
      break;
    }
    if (NULL == scene) return sceneid;
    sceneid = scene->getid();
    pushscene(scene);
    return sceneid;
  __LEAVE_FUNCTION
    return ID_INVALID;
}

bool Fuben::pushscene(scene::Scene *scene) {
  __ENTER_FUNCTION
    if (count_ >= SCENE_MAX) return false;
    scene_loadnode_[head_++].scene = scene;
    if (head_ >= SCENE_MAX) head_ = 0;
    ++count_;
    if (count_ > SCENE_MAX) return false;
    return true;
  __LEAVE_FUNCTION
    return false;
}

scene::Scene *Fuben::popscene() {
  __ENTER_FUNCTION
    if (0 == count_) return NULL;
    scene::Scene *scene = scene_loadnode_[tail_++].scene;
    if (tail_ >= SCENE_MAX) tail_ = 0;
    --count_;
    return scene;
  __LEAVE_FUNCTION
    return NULL;
}

bool Fuben::heartbeat(uint32_t time) {
  USE_PARAM(time);
  return true;
}

bool Fuben::addscene(sceneid_t sceneid, scene::Scene *scene) {
  __ENTER_FUNCTION
    if (sceneid < 0 || sceneid >= SCENE_MAX) return false;
    if (NULL == scenes_[sceneid]) scenes_[sceneid] = scene;
    return true;
  __LEAVE_FUNCTION
    return false;
}

sceneid_t Fuben::get_bakscene_id() const {
  return ID_INVALID;
}

float Fuben::get_bakscene_posx() const {
  return .0f;
}
   
float Fuben::get_bakscene_posz() const {
  return .0f;
}
  
bool Fuben::broadcast() {
  return true;
}

} //namespace manager

} //namespace scene
