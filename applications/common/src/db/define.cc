#include "common/db/define.h"

namespace common {

namespace db {

/* tables {*/
const char *kTableNameGloablData = "t_global";
/* } tables */


/* sqls { */
const char *kSqlSelectGlobalData = "SELECT poolid, data FROM %s";
const char *kSqlSaveGlobalData = "UPDATE %s SET data = %d WHERE poolid = %d";
/* } sqls */


} //namespace db

} //namespace common