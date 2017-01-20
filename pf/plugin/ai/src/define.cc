#include "ai/define.h"

namespace ai {

action_struct::action_struct() {
  id = ID_INVALID;
  action = NULL;
}

action_t &action_struct::operator = (action_t const &object) {
  id = object.id;
  action = object.action;
  return *this;
}

eventcondition_struct::eventcondition_struct() {
  id = ID_INVALID;
  value = 0;
}

info_struct::info_struct() {
  scriptid = ID_INVALID;
  logic = NULL;
}

branch_struct::branch_struct() {
  id = ID_INVALID;
  action = NULL;
}

eventhandle_struct::eventhandle_struct() {
  eventid = ID_INVALID;
}

} //namespace ai
