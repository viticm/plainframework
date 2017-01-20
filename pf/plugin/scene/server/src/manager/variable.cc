#include "scene/manager/variable.h"

namespace scene {

namespace manager {

Variable::Variable() {
  __ENTER_FUNCTION
    variableopt_.clear();
    scene_ = NULL;
  __LEAVE_FUNCTION
}

Variable::~Variable() {
  //do nothing
}

bool Variable::init(const variable_t &variable) {
  __ENTER_FUNCTION
    variableopt_.variable = variable;
    variableopt_.dirty = false;
    return true;
  __LEAVE_FUNCTION
    return false;
}

void Variable::setscene(scene::Scene *scene) {
  scene_ = scene;
}

int32_t Variable::get(int32_t index) {
  __ENTER_FUNCTION
    int32_t result = variableopt_.get(index);
    return result;
  __LEAVE_FUNCTION
    return 0;
}

void Variable::set(int32_t index, int32_t value) {
  __ENTER_FUNCTION
    variableopt_.set(index, value);
  __LEAVE_FUNCTION
} 

bool Variable::heartbeat(uint32_t time) {
  USE_PARAM(time);
  return true;
}

} //namespace manager

} //namespace scene
