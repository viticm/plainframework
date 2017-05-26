#include "pf/basic/logger.h"

#ifdef PF_OPEN_ODBC
#include "pf/db/odbc/interface.h"
#endif

#include "pf/db/manager.h"

namespace pf_db {

Manager::Manager()
  : connector_type_{kDBConnectorTypeODBC},
#ifdef PF_OPEN_ODBC
  odbc_system_{nullptr},
#endif
  isready_{false} {
}

Manager::~Manager() {
#ifdef PF_OPEN_ODBC
  safe_delete(odbc_system_);
#endif
}

bool Manager::init(const char *connection_or_dbname,
                   const char *username,
                   const char *password) {
  if (isready_) return true;
  bool result = false;
  switch (connector_type_) {
    case kDBConnectorTypeODBC: {
#ifdef PF_OPEN_ODBC
      odbc_system_ = new odbc::System();
      Assert(odbc_system_);
      result = 
        odbc_system_->init(connection_or_dbname, username, password);
      isready_ = odbc_system_->getinterface()->is_prepare();
#else
      SLOW_ERRORLOG("database", "The module odbc not open!!!");
#endif
      break;
    }
    default:
      result = false;
      break;
  }
  return result;
}

int32_t Manager::get_columncount() const {
  int32_t result = 0;
  switch (connector_type_) {
    case kDBConnectorTypeODBC:
#ifdef PF_OPEN_ODBC
      result = odbc_system_->get_columncount();
#else
      SLOW_ERRORLOG("database", "The module odbc not open!!!");
#endif
      break;
    default:
      break;
  }
  return result;
}

bool Manager::getresult() const {
  bool result = true;
  switch (connector_type_) {
    case kDBConnectorTypeODBC:
#ifdef PF_OPEN_ODBC
      result = odbc_system_->getresult();
#else
      SLOW_ERRORLOG("database", "The module odbc not open!!!");
#endif
      break;
    default:
      result = false;
      break;
  }
  return result;
}

bool Manager::query(const std::string &sql_str) {
  bool result = true;
  switch (connector_type_) {
    case kDBConnectorTypeODBC:
#ifdef PF_OPEN_ODBC
      result = odbc_system_->query(sql_str);
#else
      SLOW_ERRORLOG("database", "The module odbc not open!!!");
#endif
      break;
    default:
      result = false;
      break;
  }
  return result;
}

bool Manager::fetch(int32_t orientation, int32_t offset) {
  bool result = true;
  switch (connector_type_) {
    case kDBConnectorTypeODBC:
#ifdef PF_OPEN_ODBC
      result = odbc_system_->fetch(orientation, offset);
#else
      SLOW_ERRORLOG("database", "The module odbc not open!!!");
#endif
      break;
    default:
      result = false;
      break;
  }
  return result;
}

int32_t Manager::get_affectcount() const {
  int32_t result = 0;
  switch (connector_type_) {
    case kDBConnectorTypeODBC:
#ifdef PF_OPEN_ODBC
      result = odbc_system_->get_result_count();
#else
      SLOW_ERRORLOG("database", "The module odbc not open!!!");
#endif
      break;
    default:
      result = -1;
      break;
  }
  return result;
}

bool Manager::check_db_connect() {
  bool result = false;
  switch (connector_type_) {
    case kDBConnectorTypeODBC:
#ifdef PF_OPEN_ODBC
      result = odbc_system_->check_db_connect();
#else
      SLOW_ERRORLOG("database", "The module odbc not open!!!");
#endif
      break;
    default:
      result = false;
      break;
  }
  isready_ = result; //Check connect.
  return result;
}

float Manager::get_float(int32_t column_index, int32_t &error_code) {
  float result = 0.0f;
  switch (connector_type_) {
    case kDBConnectorTypeODBC:
#ifdef PF_OPEN_ODBC
      result = 
        odbc_system_->getinterface()->get_float(column_index, error_code);
#else
      SLOW_ERRORLOG("database", "The module odbc not open!!!");
#endif
      break;
    default:
      break;
  }
  return result;
}

int64_t Manager::get_int64(int32_t column_index, int32_t &error_code) {
  int64_t result = -1;
  switch (connector_type_) {
    case kDBConnectorTypeODBC:
#ifdef PF_OPEN_ODBC
      result = 
        odbc_system_->getinterface()->get_int64(column_index, error_code);
#else
      SLOW_ERRORLOG("database", "The module odbc not open!!!");
#endif
      break;
    default:
      break;
  }
  return result;
}

uint64_t Manager::get_uint64(int32_t column_index, int32_t &error_code) {
  uint64_t result = 0;
  switch (connector_type_) {
    case kDBConnectorTypeODBC:
#ifdef PF_OPEN_ODBC
      result = 
        odbc_system_->getinterface()->get_uint64(column_index, error_code);
#else
      SLOW_ERRORLOG("database", "The module odbc not open!!!");
#endif
      break;
    default:
      break;
  }
  return result;
}

int32_t Manager::get_int32(int32_t column_index, int32_t &error_code) {
  int32_t result = -1;
  switch (connector_type_) {
    case kDBConnectorTypeODBC:
#ifdef PF_OPEN_ODBC
      result = 
        odbc_system_->getinterface()->get_int32(column_index, error_code);
#else
      SLOW_ERRORLOG("database", "The module odbc not open!!!");
#endif
      break;
    default:
      break;
  }
  return result;
}

uint32_t Manager::get_uint32(int32_t column_index, int32_t &error_code) {
  uint32_t result = 0;
  switch (connector_type_) {
    case kDBConnectorTypeODBC:
#ifdef PF_OPEN_ODBC
      result = 
        odbc_system_->getinterface()->get_uint32(column_index, error_code);
#else
      SLOW_ERRORLOG("database", "The module odbc not open!!!");
#endif
      break;
    default:
      break;
  }
  return result;
}
   
int16_t Manager::get_int16(int32_t column_index, int32_t &error_code) {
  int16_t result = -1;
  switch (connector_type_) {
    case kDBConnectorTypeODBC:
#ifdef PF_OPEN_ODBC
      result = 
        odbc_system_->getinterface()->get_int16(column_index, error_code);
#else
      SLOW_ERRORLOG("database", "The module odbc not open!!!");
#endif
      break;
    default:
      break;
  }
  return result;
}

uint16_t Manager::get_uint16(int32_t column_index, int32_t &error_code) {
  uint16_t result = 0;
  switch (connector_type_) {
    case kDBConnectorTypeODBC:
#ifdef PF_OPEN_ODBC
      result = 
        odbc_system_->getinterface()->get_uint16(column_index, error_code);
#else
      SLOW_ERRORLOG("database", "The module odbc not open!!!");
#endif
      break;
    default:
      break;
  }
  return result;
}
   
int8_t Manager::get_int8(int32_t column_index, int32_t &error_code) {
  int8_t result = -1;
  switch (connector_type_) {
    case kDBConnectorTypeODBC:
#ifdef PF_OPEN_ODBC
      result = 
        odbc_system_->getinterface()->get_int8(column_index, error_code);
#else
      SLOW_ERRORLOG("database", "The module odbc not open!!!");
#endif
      break;
    default:
      break;
  }
  return result;
}

uint8_t Manager::get_uint8(int32_t column_index, int32_t &error_code) {
  uint8_t result = 0;
  switch (connector_type_) {
    case kDBConnectorTypeODBC:
#ifdef PF_OPEN_ODBC
      result = 
        odbc_system_->getinterface()->get_int8(column_index, error_code);
#else
      SLOW_ERRORLOG("database", "The module odbc not open!!!");
#endif
      break;
    default:
      break;
  }
  return result;
}
 
int32_t Manager::get_string(int32_t column_index, 
                          char *buffer, 
                          int32_t buffer_length, 
                          int32_t &error_code) {
  int32_t result = -1;
  switch (connector_type_) {
    case kDBConnectorTypeODBC:
#ifdef PF_OPEN_ODBC
      result = 
        odbc_system_->getinterface()->get_string(column_index,
                                                 buffer,
                                                 buffer_length,
                                                 error_code);
#else
      SLOW_ERRORLOG("database", "The module odbc not open!!!");
#endif
      break;
    default:
      break;
  }
  return result;
}

int32_t Manager::get_field(int32_t column_index, 
                         char *buffer, 
                         int32_t buffer_length, 
                         int32_t &error_code) {
  int32_t result = -1;
  switch (connector_type_) {
    case kDBConnectorTypeODBC:
#ifdef PF_OPEN_ODBC
      result = 
        odbc_system_->getinterface()->get_field(column_index,
                                                buffer,
                                                buffer_length,
                                                error_code);
#else
      SLOW_ERRORLOG("database", "The module odbc not open!!!");
#endif
      break;
    default:
      break;
  }
  return result;
}

int32_t Manager::get_binary(int32_t column_index, 
                          char *buffer, 
                          int32_t buffer_length, 
                          int32_t &error_code) {
  int32_t result = -1;
  switch (connector_type_) {
    case kDBConnectorTypeODBC:
#ifdef PF_OPEN_ODBC
      result = 
        odbc_system_->getinterface()->get_binary(column_index,
                                                 buffer,
                                                 buffer_length,
                                                 error_code);
#else
      SLOW_ERRORLOG("database", "The module odbc not open!!!");
#endif
      break;
    default:
      break;
  }
  return result;
}

int32_t Manager::get_binary_withdecompress(int32_t column_index, 
                                         char *buffer, 
                                         int32_t buffer_length, 
                                         int32_t &error_code) {
  int32_t result = -1;
  switch (connector_type_) {
    case kDBConnectorTypeODBC:
#ifdef PF_OPEN_ODBC
      result = 
        odbc_system_->getinterface()->get_binary_withdecompress(
            column_index,
            buffer,
            buffer_length,
            error_code);
#else
      SLOW_ERRORLOG("database", "The module odbc not open!!!");
#endif
      break;
    default:
      break;
  }
  return result;
}

const char *Manager::get_columnname(int32_t column_index) const {
  const char *result = nullptr;
  switch (connector_type_) {
    case kDBConnectorTypeODBC:
#ifdef PF_OPEN_ODBC
      result = 
        odbc_system_->getinterface()->get_name(column_index);
#else
      SLOW_ERRORLOG("database", "The module odbc not open!!!");
#endif
      break;
    default:
      break;
  }
  return result;
}

const char *Manager::get_data(
  int32_t column_index, const char *_default) const {
  const char *result = nullptr;
  switch (connector_type_) {
    case kDBConnectorTypeODBC:
#ifdef PF_OPEN_ODBC
      result = 
        odbc_system_->getinterface()->get_data(column_index, _default);
#else
      SLOW_ERRORLOG("database", "The module odbc not open!!!");
#endif
      break;
    default:
      break;
  }
  return result;
}

db_columntype_t Manager::gettype(int32_t column_index) {
  auto type = kDBColumnTypeString;
  switch (connector_type_) {
    case kDBConnectorTypeODBC:
#ifdef PF_OPEN_ODBC
      type = odbc_system_->gettype(column_index);
#else
      SLOW_ERRORLOG("database", "The module odbc not open!!!");
#endif
      break;
    default:
      break;
  }
  return type;
}

} //namespace pf_db
