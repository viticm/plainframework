#include "structure/object.h"
#include "ai/controller.h"
#include "ai/event.h"

namespace ai {

Event::Event() {
  __ENTER_FUNCTION
    eventcondition_table_.clear();
    conditionlogic_ = false;
    runonce_ = false;
    cdframe_ = 0;
    actionid_ = ID_INVALID;
    scriptid_ = ID_INVALID;
    isactive_ = false;
  __LEAVE_FUNCTION
}

Event::~Event() {
  eventcondition_table_.clear();
}

void Event::setaction(int16_t id) {
  __ENTER_FUNCTION
    actionid_ = id;
    scriptid_ = 0;
  __LEAVE_FUNCTION
}
   
bool Event::runaction(structure::Object *self) {
  __ENTER_FUNCTION
    Assert(self);
    return true;
  __LEAVE_FUNCTION
    return false;
}
   
bool Event::init(int16_t actionid, bool conditionlogic, bool runonce) {
  __ENTER_FUNCTION
    setaction(actionid);
    conditionlogic_ = conditionlogic;
    runonce_ = runonce;
    isactive_ = true; 
  __LEAVE_FUNCTION
    return false;
}
  
void Event::set_cdframe(int32_t cdframe) {
  cdframe_ = cdframe;
}
   
bool Event::addcondition(int16_t conditionid, int32_t value) {
  __ENTER_FUNCTION
    eventcondition_t eventcondition;
    eventcondition.id = conditionid;
    eventcondition.value = value;
    eventcondition_table_.push_back(eventcondition);
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Event::checkcondition(structure::Object *self, 
                           int32_t eventindex, 
                           int32_t current) {
  return true;
}

bool Event::checkevent(structure::Object *self, 
                       int32_t eventindex, 
                       int32_t current) {
  __ENTER_FUNCTION
    int32_t index = 0;
    int32_t size = 0;
    if (!self || !isactive_) return false;
    size = eventcondition_table_.size();
    if (0 == size) return false;
    Controller *controller = 
      reinterpret_cast<Controller *>(self->get_aicontroller());
    if (NULL == controller) return false;
    //int32_t event_nextframe = controller->get_event_nextframe(eventindex);
    bool result = false;
    if (conditionlogic_) { //条件为或
      for (index = 0; index < size; ++index) {
        result = checkcondition(self, index, current);
        if (result) break;
      }
    } else {
      for (index = 0; index < size; ++index) {
        result = checkcondition(self, index, current);
        if (!result) break;
      }
    }
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Event::is_runonce() const {
  return runonce_;
}

int32_t Event::get_cdframe() const {
  return cdframe_;
}

bool Event::fireevent(structure::Object *self, 
                      int32_t eventindex, 
                      int32_t current) {
  __ENTER_FUNCTION
    bool result = false;
    if (NULL == self || -1 == current || !isactive_) return false;
    result = checkevent(self, eventindex, current);
    if (result) result = runevent(self);
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Event::runevent(structure::Object *self) {
  __ENTER_FUNCTION
    bool result = runaction(self);
    return result;
  __LEAVE_FUNCTION
    return false;
}

} //namespace ai
