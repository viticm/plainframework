#include "pf/base/log.h"
#include "ai/define.h"
#include "ai/controller.h"

namespace ai {

Controller::Controller() {
  __ENTER_FUNCTION
    base_ = NULL;
    memset(base_memory_, 0, sizeof(base_memory_));
    isrunning_ = false;
  __LEAVE_FUNCTION
}

Controller::~Controller() {
  //do nothing
}

bool Controller::init(int8_t type, structure::Object *self) {
  __ENTER_FUNCTION
    bool result = false;
    base_ = reinterpret_cast<Base *>(base_memory_[0]);
    result = set_aitype(type);
    if (!result) return result;
    result = base_->init(self);
    if (!result) return result;
    base_->settype(type);
    return result;
  __LEAVE_FUNCTION
    return false;
}


void Controller::cleanup() {
  __ENTER_FUNCTION
    Assert(base_);
    base_->cleanup();
    base_->~Base();
    base_ = NULL;
  __LEAVE_FUNCTION
}

bool Controller::set_aitype(int8_t type) {
  __ENTER_FUNCTION
    Base *base = NULL;
    switch (type) {
      case kTypeBase:
        base = new (base_) Base();
        break;
      default:
        SLOW_WARNINGLOG(AI_MODULENAME, 
                       "[ai] type error (%d), default to base",
                       type);
        base = new (base_) Base();
        break;
    }
    bool result = NULL == base ? false : true;
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Controller::heartbeat(uint32_t time) {
  __ENTER_FUNCTION
    if (!isrunning_)return false;
    Assert(base_);
    Assert(base_->self_);
    base_->heartbeat(time);
    return true;
  __LEAVE_FUNCTION
    return false;
}
   
void Controller::stop() {
  isrunning_ = false;
}
   
void Controller::restart() {
  Assert(base_);
  isrunning_ = true;
}

bool Controller::isrunning() const {
  return isrunning_;
}

Base *Controller::getai() {
  return base_;
}

int32_t Controller::get_event_nextframe(int32_t eventindex) {
  __ENTER_FUNCTION
    if (!base_) return -1;
    if (eventindex < 0 || eventindex >= AI_EVENT_MAX) return -1;
    int32_t result = -1;
    result = base_->eventtimer_[eventindex];
    return result;
  __LEAVE_FUNCTION
    return -1;
}

} //namespace ai
