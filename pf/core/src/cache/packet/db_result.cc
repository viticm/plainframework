#include "pf/engine/kernel.h"
#include "pf/cache/repository.h"
#include "pf/cache/db_store.h"
#include "pf/cache/packet/db_result.h"

using namespace pf_cache::packet;

bool DBResult::read(pf_net::stream::Input &istream) {
  result_ = istream.read_int8();
  operate_ = istream.read_int8();
  istream.read_string(key_, sizeof(key_) - 1);

  data_size_ = 0;
  //keys
  uint8_t keycount = istream.read_uint8();
  data_size_ += sizeof(uint8_t);
  for (uint8_t i = 0; i < keycount; ++i) {
    char key[128] = {0};
    istream.read_string(key, sizeof(key) - 1);
    data_size_ += sizeof(uint32_t);
    data_size_ += static_cast<uint32_t>(strlen(key));
    pf_basic::type::variable_t variable = key;
    data_.keys.push_back(variable);
  }

  //values
  uint32_t valuecount = istream.read_uint32();
  data_size_ += sizeof(uint32_t);
  for (uint32_t i = 0; i < valuecount; ++i) {
    int8_t type = istream.read_int8();
    data_size_ += sizeof(int8_t);
    char temp[1024 * 100] = {0};
    istream.read_string(temp, sizeof(temp) - 1);
    data_size_ += sizeof(uint32_t);
    data_size_ += static_cast<uint32_t>(strlen(temp));
    pf_basic::type::variable_t variable = temp;
    variable.type = type;
    data_.values.push_back(variable);
  }
  return true;
}

bool DBResult::write(pf_net::stream::Output &ostream) {
  ostream << result_ << operate_ << key_;

  //keys
  uint32_t keysize = static_cast<uint32_t>(data_.keys.size());
  ostream.write_uint8(static_cast<uint8_t>(keysize));
  db_keys_t::iterator _iterator;
  for (uint32_t i = 0; i < keysize; ++i) {
    ostream.write_string(data_.keys[i].string());
  }

  //values
  ostream.write_uint32(static_cast<uint32_t>(data_.values.size()));
  for (uint32_t i = 0; i < data_.values.size(); ++i) {
    ostream.write_int8(data_.values[i].type);
    ostream.write_string(data_.values[i].string());
  }
  data_size_ = get_data_size();
  return true;
}

uint32_t DBResult::size() const {
  uint32_t result = 0;
  result += sizeof(result_);
  result += sizeof(operate_);
  result += 128;
  result += data_size_;
  return result;
}

uint32_t DBResult::execute(pf_net::connection::Basic *connection) {
  if (!ENGINE_POINTER) return kPacketExecuteStatusContinue;
  auto cache_manager = ENGINE_POINTER->get_cache();
  if (!cache_manager) return kPacketExecuteStatusContinue;
  auto store = 
    dynamic_cast< pf_cache::DBStore *>(cache_manager->get_db_dirver()->store());
  store->db_to_cache(get_key(), data_); 
  return kPacketExecuteStatusContinue;
}

uint32_t DBResult::get_data_size() const {
  uint32_t data_size = 0;
  //keys
  data_size += sizeof(uint8_t);
  for (uint8_t i = 0; i < data_.keys.size(); ++i) {
    data_size += sizeof(uint32_t);
    data_size += static_cast<uint32_t>(data_.keys[i].data.size());
  }

  //values
  data_size += sizeof(uint32_t);
  for (uint32_t i = 0; i < data_.values.size(); ++i) {
    data_size += sizeof(int8_t);
    data_size += sizeof(uint32_t);
    data_size += static_cast<uint32_t>(data_.values[i].data.size());
  }
  return data_size;
}
