#include "pf/base/log.h"
#include "pf/base/time_manager.h"
#include "structure/object.h"
#include "ai/define.h"
#include "ai/manager.h"
#include "ai/logic.h"
#include "ai/state.h"
#include "ai/vm.h"

namespace ai {

VM::VM() {
  cleanup();
}

VM::~VM() {
  //do nothing
}

bool VM::init(structure::Object *object, int8_t type) {
  __ENTER_FUNCTION
    cleanup();
    Logic *logic = NULL;
    if (NULL == Manager::self_) return false;
    if (ID_INVALID == current_eventid_) return false;
    Assert(object);
    self_ = object;
    if (kTypeInvalid == type) {
      logic_ = NULL;
      return false;
    }
    logic = Manager::self_->get(type);
    if (NULL == logic) {
      SLOW_ERRORLOG(AI_MODULENAME,
                    "[ai] (VM::init) ai type is invalid: %d",
                    type);
      return false;
    }
    type_ = type;
    logic_ = logic;
    int16_t initstate = logic->get_initstate();
    setstate(initstate);
    fireevent(kEventPrimaryTimer, 
              self_->getid(), 
              TIME_MANAGER_POINTER->get_tickcount());
    return true;
  __LEAVE_FUNCTION
    return false;
}

void VM::cleanup() {
  __ENTER_FUNCTION
    self_ = NULL;
    type_ = kTypeInvalid;
    logic_ = NULL;
    state_ = NULL;
    stateid_ = ID_INVALID;
    actionid_ = ID_INVALID;
    current_eventid_ = ID_INVALID;
    pending_eventid_ = ID_INVALID;
    pending_eventsource_ = ID_INVALID;
    pending_eventparam_ = -1;
  __LEAVE_FUNCTION
}

int8_t VM::gettype() const {
  return type_;
}

void VM::fireevent(int32_t eventid, int32_t eventsource, int32_t eventparam) {
  __ENTER_FUNCTION
    if (ID_INVALID == eventid) return;
    if (NULL == self_ || NULL == logic_ || NULL == state_) return;
    int32_t eventblock = current_eventid_;
    int8_t kCallCountMax = 64;
    int32_t callcount = 0;
    //int32_t scriptid = ID_INVALID;
    //scriptid = logic_->get_scriptid();
    action_t handle = state_->get_eventhandle(eventid);
    if (ID_INVALID == handle.id) return;
    if (eventblock != ID_INVALID) {
      if (ID_INVALID == pending_eventid_ || eventid == pending_eventid_) {
        pending_eventid_ = eventid;
        pending_eventsource_ = eventsource;
        pending_eventparam_ = eventparam;
      } else {
        SLOW_ERRORLOG(AI_MODULENAME,
                      "[ai] (VM::fireevent) event id(%d) blocked by %d"
                      " type: %d, object name: %s",
                      eventid,
                      eventblock,
                      pending_eventid_,
                      type_,
                      self_->getname());
      }
      return;
    }
    current_eventid_ = eventid;
    self_->get_aidata()->clear();
    self_->get_aidata()->eventsource = eventsource;
    self_->get_aidata()->eventparam = eventparam;
    while (handle.id != ID_INVALID) {
      action_t nexthandle;
      if (callcount > kCallCountMax) {
        SLOW_ERRORLOG(AI_MODULENAME,
                      "[ai] (VM::fireevent) action call to limit(%d),"
                      " type: %d, event id: %d, action id: %d",
                      kCallCountMax,
                      type_,
                      eventid,
                      handle.id);
      }
      nexthandle = logic_->callaction(self_, handle);
      if (ID_INVALID == nexthandle.id) {
        SLOW_ERRORLOG(AI_MODULENAME,
                      "[ai] (VM::fireevent) action call return error,"
                      " type: %d, event id: %d, action id: %d, object name: %s",
                      type_,
                      eventid,
                      handle.id,
                      self_->getname());
        logic_ = NULL;
        break;
      }
      handle = nexthandle;
      actionid_ = handle.id;
      ++callcount;
    }
  __LEAVE_FUNCTION
}

void VM::setstate(int16_t state) {
  __ENTER_FUNCTION
    state_ = logic_->getstate(state);
    stateid_ = state;
  __LEAVE_FUNCTION
}

void VM::set_primarytimer(int32_t frame) {
  __ENTER_FUNCTION
    primarytimer_.cleanup();
    primarytimer_.start(frame, TIME_MANAGER_POINTER->get_tickcount());
  __LEAVE_FUNCTION
}
   
void VM::set_secondarytimer(int32_t frame) {
  __ENTER_FUNCTION
    secondarytimer_.cleanup();
    secondarytimer_.start(frame, TIME_MANAGER_POINTER->get_tickcount());
  __LEAVE_FUNCTION
}

void VM::set_tertiarytimer(int32_t frame) {
  __ENTER_FUNCTION
    tertiarytimer_.cleanup();
    tertiarytimer_.start(frame, TIME_MANAGER_POINTER->get_tickcount());
  __LEAVE_FUNCTION
}

void VM::heartbeat(uint32_t time) {
  __ENTER_FUNCTION
    if (pending_eventid_ != ID_INVALID) {
      int32_t eventid = pending_eventid_;
      pending_eventid_ = ID_INVALID;
      fireevent(eventid, pending_eventsource_, pending_eventparam_);
    }

    //timers
    if (primarytimer_.isstart() && primarytimer_.counting(time)) {
      fireevent(kEventPrimaryTimer, self_->getid(), time);
    }

    if (secondarytimer_.isstart() && secondarytimer_.counting(time)) {
      fireevent(kEventSecondaryTimer, self_->getid(), time);
    }
    
    if (tertiarytimer_.isstart() && tertiarytimer_.counting(time)) {
      fireevent(kEventTertiaryTimer, self_->getid(), time);
    }
  __LEAVE_FUNCTION
}

} //namespace ai
