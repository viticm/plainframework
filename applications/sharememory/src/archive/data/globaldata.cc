#include "archive/data/globaldata.h"

using namespace archive::data;
GlobalData::GlobalData(pf_db::Manager *db_manager) {
  db_manager_ = db_manager;
  db_type_ = common::db::kTypeCharacter;
}

GlobalData::~GlobalData() {
  //do nothing
}

bool GlobalData::load() {
  __ENTER_FUNCTION
    using namespace common::db;
    Assert(db_manager_);
    db_manager_->
      get_internal_query()->
      parse(kSqlSelectGlobalData, kTableNameGlobalData);
    bool result = Interface::load();
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool GlobalData::erase() {
  __ENTER_FUNCTION
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool GlobalData::save(void *source) {
  __ENTER_FUNCTION
    using namespace pf_sys::memory::share;
    using namespace common::sharememory;
    if (!db_manager_) return false;
    UnitPool<globaldata_t> *pool = 
      static_cast<UnitPool<globaldata_t> *>(source);
    uint32_t pool_maxsize = pool->get_max_size();
    Assert(pool_maxsize == 1);
    globaldata_t *globaldata = pool->get_obj(0);
    if (!globaldata) {
      Assert(false);
      return false;
    }
    globaldata->lock(kFlagSelfRead);
    common::db::globaldata_t db_globaldata;
    common::db::globaldata_t *_db_globaldata = globaldata->data;
    if (NULL == _db_globaldata) {
      Assert(false);
      return false;
    }
    memcpy(&db_globaldata, _db_globaldata, sizeof(common::db::globaldata_t));
    _db_globaldata = &db_globaldata;
    globaldata->unlock(kFlagSelfRead);
    db_manager_
      ->get_internal_query()
      ->parse(kSqlSaveGlobalData, 
              kTableNameGloablData, 
              db_globaldata.poolid, 
              db_globaldata.data);
    bool result = Interface::save(source);
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool GlobalData::fetch(void *source) {
  __ENTER_FUNCTION
    using namespace pf_sys::memory::share;
    using namespace common::sharememory;
    if (!db_manager_) return false;
    switch (db_optiontype_) {
      case kDBOptionTypeLoad: {
        UnitPool<globaldata_t> *pool = 
          static_cast<UnitPool<globaldata_t> *>(source);
        Assert(pool);
        enum {
          kPoolId = 1,
          kData
        };
        if (!db_manager_->fetch()) return false;
        globaldata_t *globaldata = pool->get_obj(0);
        if (!globaldata) {
          Assert(false);
          return false;
        }
        common::db::globaldata_t *db_globaldata = &globaldata->data;
        int32_t errorcode = 0;
        db_globaldata->poolid = db_manager_->get_int32(kPoolId, errorcode);
        db_globaldata->data = db_manager_->get_int32(kData, errorcode);
        break;
      }
      default:
        break;
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

void GlobalData::set_serverid(int16_t id) {
  serverid_ = id;
}

int16_t GlobalData::get_serverid() const {
  return serverid_;
}
