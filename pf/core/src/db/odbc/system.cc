#ifdef PF_OPEN_ODBC

#include "pf/basic/util.h"
#include "pf/basic/logger.h"
#include "pf/basic/time_manager.h"
#include "pf/db/odbc/interface.h"
#include "pf/db/odbc/system.h"

namespace pf_db {

namespace odbc {

System::System()
  : result_count_{0},
  result_{0},
  op_type_{kDBOptionTypeInitEmpty},
  odbc_interface_{nullptr} {
}

System::~System() {
  safe_delete(odbc_interface_);
}

bool System::init(const char *connectionname,
                  const char *username,
                  const char *password) {
  bool connected = true;
  odbc_interface_ = new Interface();
  Assert(odbc_interface_);
  connected = odbc_interface_->connect(connectionname, username, password);
  if (!connected) {
    SLOW_ERRORLOG(DB_MODULENAME,
                  "[db.odbc] (System::init) failed."
                  " connectionname: %s, username: %s, password: %s,"
                  " errormessage: %s",
                  connectionname,
                  username,
                  password,
                  get_error_message());
  }
  return connected;
}

uint32_t System::get_result_count() {
  return result_count_;
}

bool System::fetch(int32_t orientation, int32_t offset) {
  bool result = odbc_interface_->fetch(orientation, offset);
  return result;
}

int32_t System::get_internal_affect_count() {
  return odbc_interface_->get_affect_row_count();
}

bool System::is_prepare() {
  return odbc_interface_->is_prepare();
}

bool System::check_db_connect() {
  Assert(odbc_interface_);
  bool result = true; //Error check with not connected.
  if (!odbc_interface_->is_connected()) {
    uint32_t tickcount = TIME_MANAGER_POINTER->get_tickcount();
    if (timer_.counting(tickcount) && odbc_interface_->connect()) {
      SLOW_DEBUGLOG("odbc",
        "[db.odbc] the connection reconnect successful!"
        " connection name: %s.",
        odbc_interface_->get_connection_name());
      timer_.cleanup();
      result = true;
    } else {
      if (!timer_.isstart()) {
        SLOW_WARNINGLOG(DB_MODULENAME,
          "[db.odbc] the connection lost, try connect after 5 seconds!"
          " connection name: %s.",
          odbc_interface_->get_connection_name());
        timer_.start(5000, tickcount);
      }
    }
  }
  return result;
}

bool System::getresult() const {
  return result_;
}

bool System::query(const std::string &sql_str) {
  if (!is_prepare()) return false;
  if (!odbc_interface_) return false;
  odbc_interface_->clear();
  op_type_ = kDBOptionTypeQuery;
  result_ = odbc_interface_->execute(sql_str);
  result_count_ = odbc_interface_->get_affect_row_count();
  return result_;
}

bool System::add_new(const std::string &sql_str) {
  if (!is_prepare()) return false;
  if (!odbc_interface_) return false;
  op_type_ = kDBOptionTypeAddNew;
  odbc_interface_->clear();
  result_ = odbc_interface_->execute(sql_str);
  result_count_ = odbc_interface_->get_affect_row_count();
  return result_;
}

bool System::_delete(const std::string &sql_str) {
  if (!is_prepare()) return false;
  if (!odbc_interface_) return false;
  op_type_ = kDBOptionTypeDelete;
  odbc_interface_->clear();
  result_ = odbc_interface_->execute(sql_str);
  result_count_ = odbc_interface_->get_affect_row_count();
  return result_;
}

bool System::save(const std::string &sql_str) {
  if (!is_prepare()) return false;
  if (!odbc_interface_) return false;
  op_type_ = kDBOptionTypeSave;
  odbc_interface_->clear();
  result_ = odbc_interface_->execute(sql_str);
  result_count_ = odbc_interface_->get_affect_row_count();
  return result_;
}

int32_t System::get_error_code() {
  return odbc_interface_->get_error_code();
}

const char *System::get_error_message() {
  return odbc_interface_->get_error_message();
}

Interface *System::getinterface() {
  return odbc_interface_;
}

int32_t System::get_columncount() const {
  int32_t result = 0;
  result = odbc_interface_->get_columncount();
  return result;
}

db_columntype_t System::gettype(int32_t column_index) {
  auto type = kDBColumnTypeString;
  auto typecode = odbc_interface_->get_type(column_index); 
  switch (typecode) {
    case SQL_CHAR:
      break;
    case SQL_VARCHAR:
      break;
    case SQL_LONGVARCHAR:
      break;
    case SQL_WCHAR:
      break;
    case SQL_WVARCHAR:
      break;
    case SQL_WLONGVARCHAR:
      break;
    case SQL_DECIMAL:
      break;
    case SQL_NUMERIC:
      type = kDBColumnTypeNumber;
      break;
    case SQL_SMALLINT:
      type = kDBColumnTypeInteger;
      break;
    case SQL_INTEGER:
      type = kDBColumnTypeInteger;
      break;
    case SQL_REAL:
      type = kDBColumnTypeNumber;
      break;
    case SQL_FLOAT:
      type = kDBColumnTypeNumber;
      break;
    case SQL_DOUBLE:
      type = kDBColumnTypeNumber;
      break;
    case SQL_BIT:
      type = kDBColumnTypeInteger;
      break;
    case SQL_TINYINT:
      type = kDBColumnTypeInteger;
      break;
    case SQL_BIGINT:
      type = kDBColumnTypeInteger;
      break;
    case SQL_TYPE_DATE:
      break;
    case SQL_TYPE_TIME:
      break;
    case SQL_TYPE_TIMESTAMP:
      break;
    case SQL_INTERVAL_MONTH:
      break;
    case SQL_INTERVAL_YEAR:
      break;
    case SQL_INTERVAL_YEAR_TO_MONTH:
      break;
    case SQL_INTERVAL_DAY:
      break;
    case SQL_INTERVAL_HOUR:
      break;
    case SQL_INTERVAL_MINUTE:
      break;
    case SQL_INTERVAL_SECOND:
      break;
    case SQL_INTERVAL_DAY_TO_HOUR:
      break;
    case SQL_INTERVAL_DAY_TO_MINUTE:
      break;
    case SQL_INTERVAL_DAY_TO_SECOND:
      break;
    case SQL_INTERVAL_HOUR_TO_MINUTE:
      break;
    case SQL_INTERVAL_HOUR_TO_SECOND:
      break;
    case SQL_INTERVAL_MINUTE_TO_SECOND:
      break;
    case SQL_GUID:
      break;
    case SQL_BINARY:
      break;
    case SQL_VARBINARY:
      break;
    case SQL_LONGVARBINARY:
      break;
    default:
      break;
  }
  return type;
}

} //namespace odbc

} //namespace pf_db

#endif
