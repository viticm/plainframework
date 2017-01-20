#include "pf/file/tab.h"
#include "pf/basic/logger.h"
#include "pf/basic/string.h"
#include "pf/basic/io.tcc"
#include "pf/db/query.h"
#include "pf/net/connection/basic.h"
#include "pf/cache/packet/db_query.h"
#include "pf/cache/db_store.h"

namespace pf_cache {

void str2array(const char *str, 
               pf_basic::type::variable_array_t &array, 
               const char *types = nullptr) {
  using namespace pf_basic;
  if (is_null(str)) return;
  std::vector< std::string > values;
  std::vector< std::string > _types;
  string::explode(str, values, "\t", true, true);
  if (!is_null(types))
    string::explode(str, _types, "\t", true, true);
  int32_t var_type = type::kVariableTypeString; 
  for (size_t i = 0; i < values.size(); ++i) {
    type::variable_t var{values[i]};
    if (_types.size() != 0 && 0 == values.size() % _types.size()) {
      type::variable_t _type{_types[i % _types.size()]};
      var_type = kDBColumnTypeNumber == _type.int32() ? 
                 type::kVariableTypeDouble : var_type; 
    }
    if (type::kVariableTypeDouble == var_type)
      var = var._double();
    array.push_back(var);
  }
}

void SharePool::generate_sql(
    const std::vector< std::string > &save_columns,
    int16_t index, 
    int16_t data_index, 
    std::string &sql) {
  db_table_base_t *t_base = table_base(index, data_index);
  db_table_info_t *t_info = table_info(index, data_index);
  db_item_t *t_item = item(index, data_index);
  if (kQueryInvalid == t_item->status || 
      kQueryError == t_item->status || 
      kQueryWaiting == t_item->status) return;
  db_query_t _sql;
  pf_db::Query query(&_sql);
  std::string table_name;
  table_name = t_base->prefix;
  table_name += t_base->name;
  query.set_tablename(table_name.c_str());
  switch (t_item->status) {
    case kQuerySelect:
    case kQueryInsert:
    case kQueryDelete:
      _sql.clear();
      _sql.concat("%s", t_item->get_data());
      break;
    default:  //kQueryUpdate
      db_fetch_array_t array;
      if ('\0' == t_info->column_names[0]) break;
      bool result = t_item->data_to_fetch_array(array, t_info);
      if (!result) break;
      auto lines = array.values.size() % array.keys.size();
      auto column_count = array.keys.size();
      for (decltype(column_count)i = 0; i < column_count; ++i) {
        pf_basic::type::variable_array_t values;
        for (decltype(lines) j = 0; j < lines; ++j) {
          uint16_t index = i * column_count + j;
          if (index > array.values.size() || 0 == array.values.size()) break;
          values.push_back(array.values[index]);
        }
        query.update(array.keys, values);
        std::string save_cond;
        for (size_t m = 0; m < save_columns.size(); ++m) {
          char temp[128]{0,};
          pf_basic::type::variable_t *var = 
            array.get(i, save_columns[m].c_str()); 
          snprintf(temp, 
                   sizeof(temp) - 1, 
                   pf_basic::type::kVariableTypeString == var->type 
                   ? "%s=\'%s\'" : "%s=%s", 
                   save_columns[m].c_str(), 
                   var->string());
          save_cond += temp;
          if (m != 0 && m != save_columns.size())
            save_cond += " and ";
        }
        save_cond += ";";
        query.where(save_cond.c_str());
      }
      break;
  }
  sql = _sql.sql_str_;
}

DBStore::DBStore() : 
  service_{false},
  ready_{false},
  query_net_{false},
  db_manager_{nullptr},
  net_manager_{nullptr},
  get_db_connection_func_{nullptr},
  cache_last_check_time_{0} {
  keys_.key_map = ID_INVALID;
  keys_.recycle_map = ID_INVALID;
  keys_.query_map = ID_INVALID;
  packet_id_.query = CACHE_SHARE_NET_QUERY_PACKET_ID;
  packet_id_.result = CACHE_SHARE_NET_RESULT_PACKET_ID;
  share_config_map_.init(100);
}

DBStore::~DBStore() {
  //do nothing.
}

bool DBStore::load_config(const std::string &file_name) {
  pf_file::Tab conf(0);
  bool ok = false;
  ok = conf.open_from_txt(file_name.c_str());
  if (!ok) return false;
  auto number = conf.get_record_number();
  for (decltype(number) i = 0; i < number; ++i) {

    //share config.
    const char *name = conf.get_fielddata(i, "index")->string_value;
    db_share_config_t share_conf;
    share_conf.size = conf.get_fielddata(i, "size")->int_value;
    share_conf.same_columns = 
      1 == conf.get_fielddata(i, "same_columns")->int_value ? true : false;
    /*
    share_conf.condition = conf.get_fielddata(i, "condition")->string_value;
    share_conf.condition_size = 
      conf.get_fielddata(i, "condition_size")->int_value;
    */
    const char *save_columns = 
      conf.get_fielddata(i, "save_columns")->string_value;
    pf_basic::string::explode(
        save_columns, share_conf.save_columns, "#", true, true);
    share_conf.no_save = 
      1 == conf.get_fielddata(i, "no_save")->int_value ? true : false;
    share_conf.save_interval = conf.get_fielddata(i, "save_interval")->int_value;
    share_conf.index = conf.get_fielddata(i, "group_index")->int_value;
    share_conf.share_key = conf.get_fielddata(i, "share_key")->int_value;
    share_conf.recycle_size = conf.get_fielddata(i, "recycle_size")->int_value;
    share_conf.name = name;
    share_conf.data_size = conf.get_fielddata(i, "data_size")->int_value;
    //share_conf.header_size = conf.get_fielddata(i, "header_size")->int_value;
    share_config_map_.add(name, share_conf);

    //group item.
    pf_sys::memory::share::group_item_t group_item;
    group_item.index = share_conf.index;
    group_item.size = share_conf.size;
    group_item.header_size = sizeof(db_table_base_t) + sizeof(db_table_info_t);
    group_item.data_size = sizeof(db_item_t) + share_conf.data_size;
    group_item.same_header = share_conf.same_columns;
    group_item.name = name;
    std::map< int32_t, 
      std::vector< pf_sys::memory::share::group_item_t > >::iterator it;
    it = share_group_map_.find(share_conf.share_key);
    if (it == share_group_map_.end()) {
      std::vector< pf_sys::memory::share::group_item_t > temp;
      temp.push_back(group_item);
      share_group_map_[share_conf.share_key] = temp;
    } else {
      it->second.push_back(group_item);
    }

    size_map_iterator it1;
    //Hash key.
    it1 = hash_size_map_.find(share_conf.share_key);
    if (it1 != hash_size_map_.end()) {
      it1->second += share_conf.size;
    } else {
      hash_size_map_.add(share_conf.share_key, share_conf.size);
    }
    
    //Recycle key.
    it1 = hash_size_map_.find(share_conf.share_key);
    if (it1 != hash_size_map_.end()) {
      char msg[1204]{0};
      snprintf(msg,
               sizeof(msg) - 1,
               "[cache] DBStore::init recycle check faild: [%s|%d]",
               name,
               share_conf.share_key);
      AssertEx(share_conf.recycle_size == it1->second, msg);
    } else {
      hash_size_map_.add(share_conf.share_key, share_conf.recycle_size);
    }
  }
  return true;
}

bool DBStore::init() {
  if (ready_) return true;
  using namespace pf_sys::memory::share;
  //Check recycle and hash key.
  if (ID_INVALID == keys_.key_map || 
      ID_INVALID == keys_.recycle_map || 
      ID_INVALID == keys_.query_map)
    return false;
  //Check config.
  if (0 == share_config_map_.getcount() || 0 == share_group_map_.size())
    return false;

  //Share pool init.
  std::map< int32_t, std::vector< group_item_t > >::iterator it;
  for (it = share_group_map_.begin(); it != share_group_map_.end(); ++it) {
    uint32_t key = static_cast<uint32_t>(it->first);
    const std::vector< group_item_t > &items = it->second;
    std::unique_ptr< SharePool > temp(new SharePool(key, items));
    if (!temp->init(service_)) {
      SLOW_ERRORLOG(CACHE_MODULENAME, 
                    "[cache] DBStore::init share pool error, key: %d", 
                    key);
      return false;
    }
    share_pool_map_[key] = std::move(temp);
  }

  size_t hash_key_count = 0;
  size_t hash_recycle_count = 0;
  size_map_iterator it1;
  for (it1 = hash_size_map_.begin(); 
       it1 != hash_size_map_.end(); 
       ++it1) {
    hash_key_count += it1->second;
  }

  for (it1 = recycle_size_map_.begin(); 
       it1 != recycle_size_map_.end(); 
       ++it1) {
    hash_recycle_count += it1->second;
  }
  //key map and recycle map init;
  uint32_t key_size = CACHE_SHARE_HASH_KEY_SIZE;
  uint32_t value_size = CACHE_SHARE_HASH_VALUE_SIZE;

  if (!key_map_.init(
        keys_.key_map, hash_key_count, key_size, value_size, service_)) {
    SLOW_ERRORLOG(CACHE_MODULENAME, 
                  "[cache] DBStore::init hash key map failed, key: %d,"
                  " size: %d",
                  keys_.key_map,
                  hash_key_count);
    return false;
  }

  if (!recycle_map_.init(
        keys_.recycle_map, hash_recycle_count, key_size, value_size, service_)) {
    SLOW_ERRORLOG(CACHE_MODULENAME, 
                  "[cache] DBStore::init hash recycle map failed, key: %d,"
                  " size: %d",
                  keys_.recycle_map,
                  hash_recycle_count);
    return false;
  }

  if (!recycle_map_.init(
        keys_.query_map, hash_recycle_count, key_size, value_size, service_)) {
    SLOW_ERRORLOG(CACHE_MODULENAME, 
                  "[cache] DBStore::init hash query map failed, key: %d,"
                  " size: %d",
                  keys_.query_map,
                  hash_recycle_count);
    return false;
  }
  ready_ = true;
  return true;
}

/**
 * key的存储结构为：表名#唯一key（如玩家ID）
 * key_map_中对应缓存的hash为：共享内存组内的数据索引#回收索引（相同共享内存key
 * 和唯一key下的一组缓存的回收索引应该一致）
 * 在回收中对应的key为：共享内存key#回收索引，回收索引不存在或为-1表示不回收
 * 共享内存MAP中存储的为唯一key（如玩家ID）
 **/ 
void *DBStore::get(const char *key) {
  using namespace pf_basic;
  if (0 == key_map_.size()) return nullptr;
  if (!cache_key_is_valid(key)) return nullptr;

  void *result = nullptr;
  cache_info_t info;
  cache_info(key, info);
  if (ID_INVALID == info.share_key || 
      INDEX_INVALID == info.share_index) return nullptr;
  share_config_iterator it_conf;
  it_conf = share_config_map_.find(info.name);
  if (it_conf == share_config_map_.end()) return nullptr;
  //Recycle remove.
  if (info.recycle_index != INDEX_INVALID)
    recycle_drop(info.share_key, info.recycle_index);
  share_pool_iterator it_pool;
  it_pool = share_pool_map_.find(info.share_key);
  if (it_pool == share_pool_map_.end()) return nullptr;
  result = reinterpret_cast<void *>(
      it_pool->second->real_data(it_conf->second.index, info.share_index));
  return result;
}

void DBStore::put(const char *key, void *value, int32_t) {
  using namespace pf_basic;
  if (!cache_key_is_valid(key)) return;
  cache_info_t info;
  cache_info(key, info);
  if (ID_INVALID == info.share_key) return;
  share_config_iterator it_conf;
  it_conf = share_config_map_.find(info.name);
  if (it_conf == share_config_map_.end()) {
    SLOW_ERRORLOG(CACHE_MODULENAME, 
                  "[cache] DBStore::put error, can't find config from name: %s", 
                  info.name.c_str());
    return;
  }

  share_pool_iterator it_pool;
  it_pool = share_pool_map_.find(it_conf->second.share_key);
  if (it_pool == share_pool_map_.end()) {
    int16_t data_index = INDEX_INVALID;
    it_pool->second->alloc(it_conf->second.index, data_index);
    if (INDEX_INVALID == data_index) return;
    char *cache = it_pool->second->real_data(it_conf->second.index, data_index);
    memset(cache, 0, it_conf->second.data_size);
    memcpy(cache, reinterpret_cast<char *>(value), it_conf->second.data_size);
    char hash[128]{0};
    snprintf(hash, sizeof(hash) - 1, "%d#%d", data_index, -1);
    key_map_.set(key, hash);
    db_item_t *cache_item = 
      it_pool->second->item(it_conf->second.index, data_index);
    memcpy(cache_item->only_key, 
           info.only_key.c_str(), 
           sizeof(cache_item->only_key) - 1);
    } else { //Cached.
    char *cache = 
      it_pool->second->real_data(it_conf->second.index, info.share_index);
    memset(cache, 0, it_conf->second.data_size);
    memcpy(cache, reinterpret_cast<char *>(value), it_conf->second.data_size);
    if (info.recycle_index != INDEX_INVALID) 
      recycle_drop(info.share_key, info.recycle_index);
  }
}

//Need add the save in forget.
void DBStore::forget(const char *key) {
  using namespace pf_basic;
  if (!cache_key_is_valid(key)) return;
  cache_info_t info;
  cache_info(key, info);
  if (ID_INVALID == info.share_key || INDEX_INVALID == info.share_index) return;
  share_config_iterator it_conf;
  it_conf = share_config_map_.find(info.name);
  if (it_conf == share_config_map_.end()) {
    SLOW_ERRORLOG(CACHE_MODULENAME, 
                  "[cache] DBStore::forget error,"
                  " can't find config from name: %s", 
                  info.name.c_str());
    return;
  }
  share_pool_iterator it_pool;
  it_pool = share_pool_map_.find(info.share_key);
  if (it_pool == share_pool_map_.end()) return;
  query(key); //Save.
  auto swap_index = 
    it_pool->second->free(it_conf->second.index, info.share_index);
  //New cache share index changed.
  if (swap_index > 0) {
    db_item_t *item = 
      it_pool->second->item(it_conf->second.index, info.share_index);
    cache_info_t swap_info;
    char swap_key[128]{0};
    snprintf(swap_key, 
             sizeof(swap_key) - 1, 
             "%s#%s", 
             it_conf->second.name.c_str(), 
             item->only_key);
    cache_info(swap_key, swap_info);
    char new_hash[128]{0};
    snprintf(new_hash, 
             sizeof(new_hash) - 1,
             "%d#%d", 
             info.share_index, swap_info.recycle_index);
    key_map_.set(swap_key, new_hash);
  } else {
    recycle_remove(info.share_key, info.only_key);
  }
  key_map_.remove(key);
}

bool DBStore::cache_key_is_valid(const char *key) {
  using namespace pf_basic;
  //Check key is valid.
  if (is_null(key)) return false;
  std::vector< std::string > array;
  string::explode(key, array, "#", true, true);
  if (array.size() != 2) return false;
  return true;
}

bool DBStore::recycle_pop(int32_t key, std::string &only_key) {
  using namespace pf_basic::type;
  char count_key[128]{0};
  snprintf(count_key, sizeof(count_key) - 1, "count_%d", key);
  const char *temp = recycle_map_[count_key];
  variable_t count = is_null(temp) ? 0 : temp;
  if (count.int32() <= 0) return false;
  char first_key[128]{0};
  snprintf(first_key, sizeof(first_key) - 1, "%d_0", key);
  variable_t val{0};
  only_key = recycle_map_[first_key];
  //Swap last key to first.
  char last_key[128]{0};
  snprintf(last_key, sizeof(last_key) - 1, "%d_%d", key, count.int32() - 1);
  recycle_map_.set(first_key, recycle_map_[last_key]);
  recycle_map_.set(last_key, 0);
  val = count.int32() - 1;
  recycle_map_.set(count_key, val.string());
  return true;
}

bool DBStore::recycle_push(int32_t key, 
                           const std::string &only_key) {
  using namespace pf_basic::type;
  char count_key[128]{0};
  snprintf(count_key, sizeof(count_key) - 1, "count_%d", key);
  const char *temp = recycle_map_[count_key];
  variable_t count = is_null(temp) ? 0 : temp;
  if (!recycle_size_map_.isfind(key)) {
    SLOW_ERRORLOG(CACHE_MODULENAME,
                  "[cache] DBStore::recycle_drop error,"
                  " can't find the recycle size key: %d",
                  key);
    return false;
  }
  auto size = recycle_size_map_.get(key);
  if (count.int32() >= size) {
#if _DEBUG
    pf_basic::io_cdebug("key: %d, recycle full, size: %d", key, size);
#endif
    return false;
  }
  auto index = count.int32();
  count += 1;
  recycle_mod(key, only_key, index);
  return true;
}

void DBStore::recycle_remove(int32_t key, const std::string &only_key) {
  using namespace pf_basic::type;
  std::map< 
    int32_t, std::vector< pf_sys::memory::share::group_item_t > >::iterator it;
  it = share_group_map_.find(key);
  if (it == share_group_map_.end()) {
    SLOW_ERRORLOG(CACHE_MODULENAME, 
                  "[cache] DBStore::recycle_remove error,"
                  " can't find config from key: %d",
                  key);
    return;
  }
  char temp[128]{0};
  std::vector< pf_sys::memory::share::group_item_t > &items = it->second;
  for (size_t i = 0; i < items.size(); ++i) {
    memset(temp, 0, sizeof(temp));
    snprintf(temp, 
             sizeof(temp) - 1, 
             "%s#%s", 
             items[i].name.c_str(), 
             only_key.c_str());
    forget(temp);
  }
}

//改变回收索引，会将此only key下所有的缓存hash更改为指定索引。
void DBStore::recycle_mod(
    int32_t key, const std::string &only_key, int32_t index) {
  using namespace pf_basic;
  group_map_iterator it;
  it = share_group_map_.find(key);
  if (it == share_group_map_.end()) {
    SLOW_ERRORLOG(CACHE_MODULENAME,
                  "[cache] DBStore::recyle_mod error,"
                  " can't find group config from key: %d",
                  key);
    return;
  }
  for (size_t i = 0; i < it->second.size(); ++i) {
    const pf_sys::memory::share::group_item_t &item = it->second[i];
    char hash_key[128]{0};
    snprintf(hash_key, 
             sizeof(hash_key) - 1, 
             "%s#%s", 
             item.name.c_str(), only_key.c_str());
    std::vector< std::string > array;
    const char *hash = key_map_[hash_key];
    string::explode(hash, array, "#", true, true);
    char new_hash[128]{0};
    snprintf(new_hash, sizeof(new_hash) - 1, "%s#%d", array[0].c_str(), index);
    key_map_.set(hash_key, new_hash);
  }
}

void DBStore::recycle_drop(int32_t key, int32_t index) {
  using namespace pf_basic;
  group_map_iterator it;
  it = share_group_map_.find(key);
  if (it == share_group_map_.end()) {
    SLOW_ERRORLOG(CACHE_MODULENAME,
                  "[cache] DBStore::recyle_drop error,"
                  " can't find group config from key: %d",
                  key);
    return;
  }

  //Check recycle is valid.
  char count_key[128]{0};
  snprintf(count_key, sizeof(count_key) - 1, "count_%d", key);
  const char *count_hash = recycle_map_[count_key];
  type::variable_t count = is_null(count_hash) ? 0 : count_hash;
  if (index > count.int32() - 1) return;
  char delete_recycle_key[128]{0};
  snprintf(delete_recycle_key, 
           sizeof(delete_recycle_key) - 1, 
           "%d_%d", 
           key, index);
  const char *delete_only_key = recycle_map_[delete_recycle_key];
  if (nullptr == delete_only_key) return;
  type::variable_t val{""};
  val = delete_only_key;
  if (val == "" || val.int32() == 0) return;

  //Recycle hash.
  auto swap_index = count.int32() - 1; 
  char swap_recycle_key[128]{0};
  snprintf(swap_recycle_key, 
           sizeof(swap_recycle_key) - 1, 
           "%d_%d", 
           key, swap_index);
  const char *swap_only_key = recycle_map_[swap_recycle_key];

  //Hash key swap all.
  recycle_mod(key, delete_only_key, -1);
  recycle_mod(key, swap_only_key, index);

  //Dec size and swap.
  val = count.int32() - 1;
  key_map_.set(count_key, val.string());
  if (index == count.int32() - 1) {
    key_map_.set(delete_recycle_key, "0");
    return;
  }
  //Recycle swap.
  key_map_.set(delete_recycle_key, swap_only_key); 
  key_map_.set(swap_recycle_key, "0");
}

void DBStore::cache_info(const char *key, cache_info_t &cache_info) {
  using namespace pf_basic;
  type::variable_t val;
  //Key.
  std::vector< std::string > array;
  string::explode(key, array, "#", true, true);
  if (array.size() != 2) return;
  cache_info.name = array[0];
  cache_info.only_key = array[1];
  share_config_iterator it_conf;
  it_conf = share_config_map_.find(cache_info.name);
  if (it_conf == share_config_map_.end()) return;
  cache_info.share_key = it_conf->second.share_key;

  array.clear();

  //Index.
  const char *hash = key_map_[key];
  if (is_null(hash)) return;
  if (array.size() != 2) return;
  val = array[0];
  cache_info.share_index = val.int32();
  val = array[1];
  cache_info.recycle_index = val.int32();
}

bool DBStore::recycle_find(int32_t key, int32_t index) {
  using namespace pf_basic;
  char recycle_key[128]{0};
  snprintf(recycle_key, sizeof(recycle_key) - 1, "%d_%d", key, index);
  const char *hash = recycle_map_[recycle_key];
  type::variable_t val;
  val = is_null(hash) ? 0 : hash;
  return val != "" && val != 0;
}
   
void DBStore::db_to_cache(const char *key, const db_fetch_array_t &data) {
  auto cache = reinterpret_cast< db_share_data_t * >(get(key));
  if (!cache) return;
  auto item = cache->data;
  cache_info_t info;
  cache_info(key, info);
  auto it_pool = share_pool_map_.find(info.share_key);
  auto it_conf = share_config_map_.find(info.name);
  auto table_info = 
    it_pool->second->table_info(it_conf->second.index, info.share_index);
  item.fetch_array_to_data(data, table_info);
}

bool DBStore::query(const char *key) {
  auto cache = reinterpret_cast< db_share_data_t * >(get(key));
  if (!cache) return false;
  cache_info_t info;
  cache_info(key, info);
  auto it_pool = share_pool_map_.find(info.share_key);
  auto it_conf = share_config_map_.find(info.name);
  std::string sql{""};
  it_pool
    ->second
    ->generate_sql(it_conf->second.save_columns, 
                   it_conf->second.index, 
                   info.share_index, 
                   sql);
  if ("" == sql) return false;
  if (query_net_ && get_db_connection_func_) {
    auto db_connection = get_db_connection_func_(*cache); 
    if (db_connection) {
      packet::DBQuery packet;
      packet.set_type(cache->data.status); //Query status.
      packet.set_id(packet_id_.query);
      packet.set_sql_str(sql.c_str());
      return db_connection->send(&packet);
    }
  } else {
    if (db_manager_) {
      db_query_t db_query;
      db_query.parse(sql.c_str());
      pf_db::Query query(&db_query);
      if (!query.init(db_manager_)) return false;
      if (!query.execute()) return false;
      db_fetch_array_t db_fetch_array;
      if (!query.fetcharray(db_fetch_array)) return false;
      cache->data.status = kQuerySuccess;
      db_to_cache(key, db_fetch_array);
      return true;
    }
  }
  return false;
}

void DBStore::flush() {
  using namespace pf_sys::memory;
  if (!service_) return;
  share::map_iterator it;
  for (it = key_map_.begin(); it != key_map_.end(); ++it)
    forget(it.first);
}

void DBStore::tick() {
  using namespace pf_sys::memory;
  //For query.
  share::map_iterator it;
  for (it = query_map_.begin(); it != query_map_.end(); ++it)
    query(it.first);
  static uint32_t check_time = CACHE_SHARE_DEFAULT_MINUTES * 60;
  auto current_time = TIME_MANAGER_POINTER->get_current_time();
  if (current_time - cache_last_check_time_ > check_time) {
    //Check live.
    for (it = key_map_.begin(); it != key_map_.end(); ++it) {
      auto cache = reinterpret_cast< db_share_data_t * >(get(it.first));
      if (cache->data.hook_time > 0 && cache->data.hook_time < current_time)
        forget(it.first);
    }
  }
}

} //namespace pf_cache
