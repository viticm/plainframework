#include "pf/basic/string.h"
#include "pf/db/manager.h"
#include "pf/db/query.h"

namespace pf_db {

Query::Query()
  : tablename_{0},
  manager_{nullptr},
  db_query_{nullptr},
  isready_{false} {
}

Query::Query(db_query_t *db_query)
  : tablename_{0},
  manager_{nullptr},
  db_query_{db_query},
  isready_{false} {
  if (!is_null(db_query_)) isready_ = true;
}

   
Query::~Query() {
  //do nothing
}

bool Query::init(Manager *manager) {
  if (nullptr == manager || !manager->get_internal_query()) return false;
  manager_ = manager;
  isready_ = true;
  return true;
}

Manager *Query::getmanager() {
  return manager_;
}

db_query_t *Query::get_db_query() {
  db_query_t *result = nullptr;
  result = is_null(db_query_) ? manager_->get_internal_query() : db_query_;
  return result;
}

void Query::set_tablename(const char *tablename) {
  pf_basic::string::safecopy(tablename_, tablename, sizeof(tablename_));
}

bool Query::execute() {
  if (!isready_ || is_null(manager_)) return false;
  bool result = manager_->query();
  return result;
}

bool Query::select(const char *string) {
  if (!isready_) return false;
  db_query_t *db_query = get_db_query();
  db_query->clear();
  db_query->concat("select %s", string);
  return true;
}

bool Query::_delete(const char *string) {
  if (!isready_) return false;
  db_query_t *db_query = get_db_query();
  db_query->clear();
  db_query->concat("delete %s", string);
  return true;
}
   
bool Query::where(const char *string) {
  if (!isready_) return false;
  db_query_t *db_query = get_db_query();
  db_query->concat(" where %s", string);
  return true;
}

bool Query::select(const pf_basic::type::variable_array_t &values) {
  if (!isready_) return false;
  uint32_t count = static_cast<uint32_t>(values.size());
  if (0 == count) return false;
  db_query_t *db_query = get_db_query();
  db_query->clear();
  db_query->concat("select");
  for (uint32_t i = 0; i < count; ++i) {
    db_query->concat(" %s%s", 
                     values[i].string(), 
                     i == count - 1 ? "" : ",");
  }
  return true;
}
   
bool Query::_delete() {
  if (!isready_) return false;
  db_query_t *db_query = get_db_query();
  db_query->clear();
  db_query->concat("delete");
  return true;
}

bool Query::insert(const pf_basic::type::variable_array_t &keys, 
                   const pf_basic::type::variable_array_t &values) {
  if (!isready_) return false;
  if (0 == strlen(tablename_)) return false;
  uint32_t keycount = static_cast<uint32_t>(keys.size());
  uint32_t valuecount = static_cast<uint32_t>(values.size());
  if (0 == keycount || keycount != valuecount) return false;
  db_query_t *db_query = get_db_query();
  db_query->clear();
  uint32_t i;
  db_query->concat("insert into %s", tablename_);
  db_query->concat(" (");
  for (i = 0; i < keycount; ++i) {
    db_query->concat("%s%s",
                     keys[i].string(),
                     i == keycount - 1 ? "" : ", ");
  }
  db_query->concat(") values (");
  for (i = 0; i < keycount; ++i) { 
    db_query->concat(pf_basic::type::kVariableTypeString == values[i].type ? 
                     "\'%s\'%s" : 
                     "%s%s",
                     values[i].string(),
                     i == keycount - 1 ? "" : ", ");
  }
  db_query->concat(")");
  return true;
}

bool Query::update(const pf_basic::type::variable_array_t &keys, 
                   const pf_basic::type::variable_array_t &values) {
  if (!isready_) return false;
  if (0 == strlen(tablename_)) return false;
  uint32_t keycount = static_cast<uint32_t>(keys.size());
  uint32_t valuecount = static_cast<uint32_t>(values.size());
  if (0 == keycount || keycount != valuecount) return false;
  db_query_t *db_query = get_db_query();
  db_query->clear();
  uint32_t i;
  db_query->concat("update %s set ", tablename_);
  for (i = 0; i < keycount; ++i) { 
    db_query->concat(pf_basic::type::kVariableTypeString == values[i].type ? 
                     "%s=\'%s\'%s" : 
                     "%s=%s%s",
                     keys[i].string(),
                     values[i].string(),
                     i == keycount - 1 ? "" : ", ");
  }
  return true;
}
   
bool Query::from() {
  if (!isready_) return false;
  if (0 == strlen(tablename_)) return false;
  db_query_t *db_query = get_db_query();
  db_query->concat(" from %s", tablename_);
  return true;
}
 
bool Query::where(const pf_basic::type::variable_t &key, 
                  const pf_basic::type::variable_t &value, 
                  const char *operator_str) {
  if (!isready_) return false;
  db_query_t *db_query = get_db_query();
  db_query->concat(" where ");
  db_query->concat(pf_basic::type::kVariableTypeString == value.type ? 
                   "%s%s\'%s\'" : 
                   "%s%s%s",
                   key.string(),
                   operator_str,
                   value.string());
  return true;
}

bool Query::_and(const pf_basic::type::variable_t &key, 
                 const pf_basic::type::variable_t &value, 
                 const char *operator_str) {
  if (!isready_) return false;
  db_query_t *db_query = get_db_query();
  db_query->concat(" and ");
  db_query->concat(pf_basic::type::kVariableTypeString == value.type ? 
                   "%s%s\'%s\'" : 
                   "%s%s%s",
                   key.string(),
                   operator_str,
                   value.string());
  return true;
}

bool Query::_or(const pf_basic::type::variable_t &key, 
                const pf_basic::type::variable_t &value, 
                const char *operator_str) {
  if (!isready_) return false;
  db_query_t *db_query = get_db_query();
  db_query->concat(" or ");
  db_query->concat(pf_basic::type::kVariableTypeString == value.type ? 
                   "%s%s\'%s\'" : 
                   "%s%s%s",
                   key.string(),
                   operator_str,
                   value.string());
  return true;
}
   
bool Query::limit(int32_t m, int32_t n) {
  if (!isready_) return false;
  db_query_t *db_query = get_db_query();
  if (0 == n) {
    db_query->concat(" limit %d", m);
  } else {
    db_query->concat(" limit %d, %d", m, n);
  }
  return true;
}

bool Query::fetcharray(db_fetch_array_t &db_fetch_array) {
  using namespace pf_basic::type;
  if (!isready_ || is_null(manager_)) return false;
  if (!manager_->fetch()) return false;
  int32_t columncount = manager_->get_columncount();
  if (columncount <= 0) return false;
  int32_t i = 0;
  //read keys
  for (i = 0; i < columncount; ++i) {
    const char *columnname = manager_->get_columnname(i);
    db_fetch_array.keys.push_back(columnname);
  }
  //read values
  do {
    for (i = 0; i < columncount; ++i) {
      variable_t value = manager_->get_data(i, "");
      int8_t columntype = manager_->gettype(i);
      value.type = static_cast<int8_t>(kDBColumnTypeString) == columntype ? 
                   static_cast<int8_t>(kVariableTypeString) :
                   static_cast<int8_t>(kVariableTypeNumber);
      db_fetch_array.values.push_back(value);
    }
  } while (manager_->fetch());
  return true;
}

} //namespace pf_db
