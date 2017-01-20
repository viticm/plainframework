#include "pf/basic/string.h"
#include "pf/cache/db_define.h"

using namespace pf_cache;

bool db_item_struct::data_to_fetch_array(db_fetch_array_t &array, 
                                         db_table_info_struct *info) {
  using namespace pf_basic;
  auto data = get_data();
  if (is_null(data)) return false;
  std::vector< std::string > values;
  std::vector< std::string > types;
  std::vector< std::string > names;
  string::explode(data, values, "\t", true, true);
  string::explode(info->column_names, names, "\t", true, true);
  for (size_t i = 0; i < names.size(); ++i) {
    array.keys.push_back(names[i]);
  }
  int32_t var_type = type::kVariableTypeString; 
  for (size_t i = 0; i < values.size(); ++i) {
    type::variable_t var{values[i]};
    if (types.size() != 0 && 0 == values.size() % types.size()) {
      type::variable_t type{types[i % types.size()]};
      var_type = kDBColumnTypeNumber == type.int32() ? 
                 type::kVariableTypeDouble : var_type; 
    }
    if (type::kVariableTypeDouble == var_type)
      var = var._double();
    array.values.push_back(var);
  }
  return true;
}

bool db_item_struct::fetch_array_to_data(const db_fetch_array_t &array, 
                                         db_table_info_struct *info) {
  using namespace pf_basic;
  auto data = get_data();
  std::string names{""};
  for (size_t i = 0; i < array.keys.size(); ++i) {
    names += array.keys[i].string();
    if (i != array.keys.size() - 1) {
      names += "\t";
    }
  }
  memcpy(info->column_names, 
         names.c_str(), 
         CACHE_DB_TABLE_COLUMN_NAMES_SIZE - 1);
  std::string types;
  std::string values;
  for (size_t i = 0; i < array.values.size(); ++i) {
    if (i < array.keys.size()) {
      type::variable_t type{array.values[i].type};
      types += type.string();
      if (i != array.keys.size() - 1) types += "\t";
    }
    values += array.values[i].string();
    if (i != array.values.size() - 1) values += "\t";
  }
  memcpy(info->column_types,
         types.c_str(),
         CACHE_DB_TABLE_COLUMN_TYPES_SIZE - 1);
  memcpy(data, values.c_str(), size - 1);
  return true;
}

bool db_item_struct::is_valid_fetch_array(const db_fetch_array_t &array, 
                                          db_table_info_struct *info) {
  db_keys_t::iterator _iterator;
  if (array.values.size() != 0) {
    if ((array.values.size() % array.keys.size()) != 0) return false;
    if (array.size() < 1) return false;
  }
  auto key_size = array.keys.size();
  size_t key_length = 0;
  for (size_t i = 0; i < key_size; ++i) {
    if (!is_null(strstr("\t", array.keys[i].string()))) return false;
    key_length += strlen(array.keys[i].string());
  }
  if (key_length > CACHE_DB_TABLE_COLUMN_NAMES_SIZE - 1) return false;
  size_t data_length = 0;
  for (size_t i = 0; i < array.values.size(); ++i) {
    if (!is_null(strstr("\t", array.values[i].string()))) return false;
    data_length += strlen(array.values[i].string());
  }
  bool result = data_length < size;
  return result;
}
