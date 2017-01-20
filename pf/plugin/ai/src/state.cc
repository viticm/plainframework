#include "ai/logic.h"
#include "ai/state.h"

namespace ai {

State::State(Logic *logic) {
  eventhandles_.clear();
  setlogic(logic);
}

State::~State() {
  eventhandles_.clear();
}

void State::setlogic(Logic *logic) {
  logic_ = logic;
}

action_t State::get_eventhandle(int32_t eventid) {
    action_t handle;
  __ENTER_FUNCTION
    if (NULL == logic_) {
      Assert(false);
      return handle;
    }
    for (int32_t i = 0; i < static_cast<int32_t>(eventhandles_.size()); ++i) {
      if (eventhandles_[i].eventid == eventid) {
        if (!eventhandles_[i].actioninfo.action) {
          Action *action = NULL;
          action = logic_->getaction(eventhandles_[i].actioninfo.id);
          Assert(action);
          eventhandles_[i].actioninfo.action = action;
        }
        handle = eventhandles_[i].actioninfo;
        break;
      }
    }
    return handle;
  __LEAVE_FUNCTION
    return handle;
}

void State::add_eventhandle(int32_t eventid, int32_t actionid) {
  __ENTER_FUNCTION
    if (ID_INVALID == eventid || ID_INVALID == actionid) return;
    for (int32_t i = 0; i < static_cast<int32_t>(eventhandles_.size()); ++i) {
      if (eventhandles_[i].eventid == eventid) return;
    }
    eventhandle_t eventhandle;
    eventhandle.eventid = eventid;
    eventhandle.actioninfo.id = actionid;
    eventhandles_.push_back(eventhandle);
  __LEAVE_FUNCTION
}

} //namespace ai
