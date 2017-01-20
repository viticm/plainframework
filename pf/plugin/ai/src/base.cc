#include "ai/event.h"
#include "ai/template.h"
#include "ai/base.h"

namespace ai {

Base::Base() {
  __ENTER_FUNCTION
    isinit_ = false;
    type_ = kTypeInvalid;
    state_ = -1;
    mainstate_ = -1;
    for (int8_t i = 0; i < AI_PARAM_COUNT; ++i) params_[i] = -1;
    for (int8_t i = 0; i < AI_EVENT_MAX; ++i) eventtimer_[i] = -1;
    interval_ = -1;
    self_ = NULL;
    template_ = NULL;
  __LEAVE_FUNCTION
}

Base::~Base() {
  //do nothing
}

bool Base::init(structure::Object *self) {
  __ENTER_FUNCTION
    self_ = self;
    return true;
  __LEAVE_FUNCTION
    return false;
}

void Base::cleanup() {
  //do nothing
}

bool Base::isinit() const {
  return isinit_;
}

int8_t Base::gettype() const {
  return type_;
}

void Base::settype(int8_t type) {
  type_ = type;
}
   
void Base::setinterval(int32_t interval) {
  interval_ = interval;
}

void Base::setinterval(float rate) {
  //do nothing
}
   
void Base::set_mainstate(int16_t state) {
  mainstate_ = state;
}
   
int16_t Base::get_mainstate() const {
  return mainstate_;
}
   
int16_t Base::getstate() const {
  return state_;
}
   
structure::position_t *Base::get_origin_position() {
  return &origin_position_;
}
   
void Base::set_origin_position(structure::position_t *position) {
  origin_position_ = *position;
}
   
void Base::setparam(int32_t index, int32_t value) {
  __ENTER_FUNCTION
    if (index > 0 && index < AI_PARAM_COUNT)
      params_[index] = value;
  __LEAVE_FUNCTION
}

void Base::checkevent(uint32_t time) {
  __ENTER_FUNCTION
    if (!template_) return;
    for (int8_t index = 0; index < AI_EVENT_MAX; ++index) {
      if (!template_->events_[index].checkevent(self_, index)) continue;
      template_->events_[index].runevent(self_);
      if (template_->events_[index].is_runonce()) {
        eventtimer_[index] = -1;
      } else {
        eventtimer_[index] = template_->events_[index].get_cdframe() + time;
      }
    }
  __LEAVE_FUNCTION
}

void Base::fireevent(int16_t eventcondition_id) {
  __ENTER_FUNCTION
    if (!template_) return;
    for (int8_t i = 0; i < AI_EVENT_MAX; ++i)
      template_->events_[i].fireevent(self_, i, eventcondition_id);
  __LEAVE_FUNCTION
}

} //namespace ai
