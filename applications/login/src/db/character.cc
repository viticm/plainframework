#include "common/db/define.h"
#include "engine/system.h"
#include "db/character.h"

namespace db {

namespace character {

uint8_t getnumber(const char *account) {
  __ENTER_FUNCTION
    using namespace common::db;
    if (!ENGINE_SYSTEM_POINTER || !ENGINE_SYSTEM_POINTER->get_dbmanager())
      return 0;
    enum {
      kDBCount = 1,
    };
    ENGINE_SYSTEM_POINTER
      ->get_dbmanager()
      ->get_internal_query()
      ->parse(kSqlGetRoleNumber, kTableNameRole, account);
    uint8_t count = 0;
    bool query_result = ENGINE_SYSTEM_POINTER->get_dbmanager()->query();
    if (query_result && ENGINE_SYSTEM_POINTER->get_dbmanager()->fetch()) { 
      int32_t errorcode = 0;
      count = 
        ENGINE_SYSTEM_POINTER->get_dbmanager()->get_uint8(kDBCount, errorcode);
    }
    return count;
  __LEAVE_FUNCTION
    return 0;
}

} //namespace character

} //namespace db
