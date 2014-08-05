#include "pf/base/log.h"
#include "common/sharememory/define.h"
#include "common/setting.h"
#include "engine/system.h"
#include "archive/data/globaldata.h"
#include "archive/node/logic.h"

//这里是共享内存节点（单元）模板逻辑函数相应实现的文件，没有其自己的命名空间

namespace archive {

namespace node {

using namespace common::sharememory;

/* global { */
template <>
bool Logic<globaldata_t>::init_after() {
  __ENTER_FUNCTION
    if (!pool_) {
      Assert(pool_);
      return false;
    }
    if (g_cmd_model == kCmdModelClearAll) return true;
    final_savetime_ = TIME_MANAGER_POINTER->get_current_time();
    uint32_t poolsize_max = pool_->get_max_size();
    USE_PARAM(poolsize_max);
    Assert(1 == poolsize_max);
    uint32_t key = pool_->get_key();
    globaldata_t *globaldata = pool_->get_obj(0);
    if (!globaldata) {
      Assert(globaldata);
      return false;
    }
    globaldata->headclear();
    /**
    int16_t serverid = SETTING_POINTER->get_server_id_by_share_memory_key(key);
    if (ID_INVALID == serverid) {
      AssertEx(false, "check server if open share memory");
    }
    **/
    if (!ENGINE_SYSTEM_POINTER || !ENGINE_SYSTEM_POINTER->get_dbmanager())
      return false;
    archive::data::GlobalData 
      globaldata_obj(ENGINE_SYSTEM_POINTER->get_dbmanager());
    globaldata_obj.set_serverid(0); //serverid);
    common::db::globaldata_t db_globaldata;
    if (globaldata_obj.load()) {
      globaldata_obj.fetch(&db_globaldata);
    } else {
      globaldata->data.poolid = 1000;
      globaldata->data.data = 999;
    }
    isready_ = true;
    SLOW_LOG(APPLICATION_NAME, 
             "[archive.node.logic] (GlobalData::after_init)"
             " key: %d, poolid: %d, data: %d", 
             key, 
             globaldata->data.poolid, 
             globaldata->data.data);
    return true;
  __LEAVE_FUNCTION
    return false;
}

template <>
bool Logic<globaldata_t>::empty() {
  __ENTER_FUNCTION
    return true;
  __LEAVE_FUNCTION
    return false;
}

template <>
bool Logic<globaldata_t>::fullflush(bool force, bool servercrash) {
  __ENTER_FUNCTION
    USE_PARAM(servercrash);
    USE_PARAM(force);
    uint32_t currenttime = TIME_MANAGER_POINTER->get_current_time();
    uint32_t savetime_interval = 
      SETTING_POINTER->share_memory_info_.center_data_save_interval;
    if (currenttime > final_savetime_ + savetime_interval) {
      if (isready_) fullflush(false);
      final_savetime_ = currenttime;
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

template <>
bool Logic<globaldata_t>::tickflush() {
  __ENTER_FUNCTION
    if (!pool_) {
      Assert(pool_);
      return false;
    }
    final_savetime_ = TIME_MANAGER_POINTER->get_current_time();
    uint32_t poolsize_max = pool_->get_max_size();
    USE_PARAM(poolsize_max);
    Assert(1 == poolsize_max);
    uint32_t key = pool_->get_key();
    common::sharememory::globaldata_t *globaldata = pool_->get_obj(0);
    if (!globaldata) {
      Assert(globaldata);
      return false;
    }
    int16_t serverid = SETTING_POINTER->get_server_id_by_share_memory_key(key);
    if (ID_INVALID == serverid) {
      AssertEx(false, "check server if open share memory");
    }
    if (!ENGINE_SYSTEM_POINTER || !ENGINE_SYSTEM_POINTER->get_dbmanager())
      return false;
    archive::data::GlobalData 
      globaldata_obj(ENGINE_SYSTEM_POINTER->get_dbmanager());
    globaldata_obj.set_serverid(serverid);
    common::db::globaldata_t db_globaldata = globaldata->data;
    int32_t result = 0;
    if (globaldata_obj.save(&db_globaldata.data)) {
      globaldata_obj.fetch(&result);
    } else {
      Assert(false);
    }
    SLOW_LOG(APPLICATION_NAME, 
             "[archive.node.logic] (GlobalData::tickflush) success!"
             " key: %d, poolid: %d, data: %d", 
             key, 
             db_globaldata.poolid, 
             db_globaldata.data);
    return true;
  __LEAVE_FUNCTION
    return false;
}
/* } global */

} //namespace node

} //namespace archive
