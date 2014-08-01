#include "common/sharememory/define.h"
#include "common/setting.h"
#include "archive/nodelogic.h"

int32_t g_cmd_model = 0;

namespace archive {

/* global { */
template <>
bool NodeLogic<globaldata_t>::init_after() {
  __ENTER_FUNCTION
    if (!pool_) {
      Assert(pool_);
      return false;
    }
    if (g_cmd_model == kCmdModelClearAll) return true;
    final_savetime_ = TIME_MANAGER_POINTER->get_current_time();
    uint32_t poolsize_max = pool_->get_max_size();
    uint64_t key = pool_->get_key();
    common::sharememory::globaldata_t *globaldata = pool_->get_obj(0);
    if (!globaldata) {
      Assert(globaldata);
      return false;
    }
    globaldata->clear();
        
    return true;
  __LEAVE_FUNCTION
    return false;
}

template <>
bool NodeLogic<globaldata_t>::empty() {
  __ENTER_FUNCTION
    return true;
  __LEAVE_FUNCTION
    return false;
}

template <>
bool NodeLogic<globaldata_t>::fullflush(bool force, bool servercrash) {
  __ENTER_FUNCTION
    return true;
  __LEAVE_FUNCTION
    return false;
}

template <>
bool NodeLogic<globaldata_t>::tickflush() {
  __ENTER_FUNCTION
    return true;
  __LEAVE_FUNCTION
    return false;
}

/* } global */

} //namespace archive
