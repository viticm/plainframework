#include "pf/db/manager.h"
#include "main.h"

int32_t main(int32_t argc, char * argv[]) {
  pf_db::Manager db_manager;
  bool result = true;
  result = db_manager.init("sword_user", "root", "mysql");
  if (!result) return 1;
  if (!db_manager.isready()) return 1;
  enum {
    kDBId = 1,
    kDBName,
  };
  db_manager.get_internal_query()
    ->parse("select id, name from users where name = 'viticm'");
  db_manager.query();
  db_manager.fetch();
  int32_t error_code = 0;
  char value[64] = {0};
  int32_t get_code = 
    db_manager.get_string(kDBName, value, sizeof(value) - 1, error_code);
  DEBUGPRINTF("get_code: %d, value: %s", get_code, value);
  return 0;
}
