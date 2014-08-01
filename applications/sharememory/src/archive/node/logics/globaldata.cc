#include "common/sharememory/define.h"
#include "common/setting.h"
#include "engine/system.h"
#include "archive/node/logic.h"

//这里是共享内存节点（单元）模板逻辑函数相应实现的文件，没有其自己的命名空间

namespace archive {

namespace node {

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
    int16_t serverid = SETTING_POINTER->get_server_id_by_share_memory_key(key);
    if (ID_INVALID == serverid) {
      AssertEx(false, "check server if open share memory");
    }
    if (!ENGINE_SYSTEM_POINTER || !ENGINE_SYSTEM_POINTER->get_dbmanager())
      return false;
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

} //namespace node

} //namespace archive
