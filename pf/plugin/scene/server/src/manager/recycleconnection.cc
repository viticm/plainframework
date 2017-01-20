#include "scene/manager/recycleconnection.h"

namespace scene {

namespace manager {

RecycleConnection::RecycleConnection() {
  sceneid_ = ID_INVALID;
}

RecycleConnection::~RecycleConnection() {
  //do nothing
}

bool RecycleConnection::heartbeat(uint32_t time) {
  USE_PARAM(time);
  return true;
}

void RecycleConnection::set_sceneid(sceneid_t sceneid) {
  sceneid_ = sceneid;
}
   
sceneid_t RecycleConnection::get_sceneid() const {
  return sceneid_;
}

} //namespace manager

} //namespace scene
