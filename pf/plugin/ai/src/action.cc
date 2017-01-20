#include "ai/action.h"

namespace ai {

Action::Action() {
  __ENTER_FUNCTION
    key_ = ID_INVALID;
    for (int8_t i = 0; i < AI_PARAM_COUNT; ++i)
      params_[i] = -1;
  __LEAVE_FUNCTION
}

Action::~Action() {
  //do nothing
}

void Action::setbranch(int16_t index, branch_t *branch) {
  __ENTER_FUNCTION
    if (index < 0 || index >= AI_ACTION_BRANCH_COUNT) return;
    branchs_[index].id = branch->id;
    branchs_[index].action = branch->action;
  __LEAVE_FUNCTION
}
   
void Action::setparam(int8_t index, int32_t value) {
  __ENTER_FUNCTION
    if (index < 0 || index >= AI_PARAM_COUNT) return;
    params_[index] = value;
  __LEAVE_FUNCTION
}

} //namespace ai
