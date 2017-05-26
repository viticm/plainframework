#ifdef PF_OPEN_ODBC

#include "pf/basic/logger.h"
#include "pf/basic/util.h"
#include "pf/basic/string.h"
#include "pf/basic/time_manager.h"
#include "pf/db/odbc/interface.h"

namespace pf_db {

namespace odbc {

Interface::Interface()
  : connected_{false},
  sql_henv_{nullptr},
  sql_hdbc_{nullptr},
  result_{SQL_SUCCESS},
  connection_name_{0},
  user_{0},
  password_{0}, 
  affect_count_{-1},
  sql_hstmt_{nullptr},
  column_count_{0},
  column_names_{nullptr},
  column_type_{nullptr},
  column_typenames_{nullptr},
  column_size_{nullptr},
  column_precision_{nullptr},
  column_nullable_{nullptr},
  column_values_{nullptr},
  column_valuelengths_{nullptr},
  error_code_{0},
  error_message_{0} {
  column_info_allocator_.malloc(COLUMN_INFO_BUFFER_MAX);
  column_value_allocator_.malloc(COLUMN_VALUE_BUFFER_MAX);
}

Interface::~Interface() {
  close();
  if (sql_hstmt_) SQLFreeHandle(SQL_HANDLE_STMT, sql_hstmt_);
  if (sql_hdbc_) SQLDisconnect(sql_hdbc_);
  if (sql_hdbc_) SQLFreeHandle(SQL_HANDLE_DBC, sql_hdbc_);
  if (sql_henv_) SQLFreeHandle(SQL_HANDLE_ENV, sql_henv_);
}

bool Interface::connect(const char *connection_name,
                        const char *user,
                        const char *password) {
  using namespace pf_basic;
  close(); //first disconnect
  if (connection_name != nullptr)
    string::safecopy(connection_name_, 
                     connection_name, 
                     sizeof(connection_name_));
  if (user != nullptr) string::safecopy(user_, user, sizeof(user_));
  if (password != nullptr)
    string::safecopy(password_, password, sizeof(password_));
  SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &sql_henv_);
  SQLSetEnvAttr(sql_henv_, 
                SQL_ATTR_ODBC_VERSION, 
                reinterpret_cast<SQLPOINTER>(SQL_OV_ODBC3), 
                SQL_IS_INTEGER);
  SQLAllocHandle(SQL_HANDLE_DBC, sql_henv_, &sql_hdbc_);
  result_ = SQLConnect(sql_hdbc_,
                       reinterpret_cast<SQLCHAR*>(connection_name_),
                       SQL_NTS,
                       reinterpret_cast<SQLCHAR*>(user_),
                       SQL_NTS,
                       reinterpret_cast<SQLCHAR*>(password_),
                       SQL_NTS);
  if (SQL_SUCCESS != result_ && SQL_SUCCESS_WITH_INFO != result_) {
    char log_buffer[512];
    memset(log_buffer, '\0', sizeof(log_buffer));
    snprintf(log_buffer, 
             sizeof(log_buffer) - 1,
             "connection name: %s, connect username: %s, password: %s", 
             connection_name_,
             user_,
             password);
    SLOW_ERRORLOG(DB_MODULENAME, 
                  "[db.odbc] (Interface::connect) failed. %s", 
                  log_buffer);
    diag_state();
    return false;
  }
  result_ = SQLAllocHandle(SQL_HANDLE_STMT, sql_hdbc_, &sql_hstmt_);
  if (result_ != SQL_SUCCESS && result_ != SQL_SUCCESS_WITH_INFO) {
    sql_hstmt_ = nullptr;
    return false;
  }
  connected_ = true;
  return true;
}

bool Interface::connect() {
  close(); //first disconnect
#ifdef MUST_CLOSE_HENV_HANDLE
  SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &sql_henv_);
  SQLSetEnvAttr(sql_henv_, 
                SQL_ATTR_ODBC_VERSION, 
                static_cast<SQLPOINTER>(SQL_OV_ODBC3), 
                SQL_IS_INTEGER);
#endif
  SQLAllocHandle(SQL_HANDLE_DBC, sql_henv_, &sql_hdbc_);
  result_ = SQLConnect(sql_hdbc_,
                       reinterpret_cast<SQLCHAR*>(connection_name_),
                       SQL_NTS,
                       reinterpret_cast<SQLCHAR*>(user_),
                       SQL_NTS,
                       reinterpret_cast<SQLCHAR*>(password_),
                       SQL_NTS);
  if (result_ != SQL_SUCCESS && result_ != SQL_SUCCESS_WITH_INFO) {
    char log_buffer[512];
    memset(log_buffer, '\0', sizeof(log_buffer));
    snprintf(log_buffer, 
             sizeof(log_buffer) - 1,
             "connection name: %s connect user: %s", 
             connection_name_,
             user_); 
    SLOW_ERRORLOG(DB_MODULENAME, 
                  "[db.odbc] (Interface::connect) failed, %s", 
                  log_buffer);
    diag_state();
    return false;
  }
  result_ = SQLAllocHandle(SQL_HANDLE_STMT, sql_hdbc_, &sql_hstmt_);
  if (result_ != SQL_SUCCESS && result_ != SQL_SUCCESS_WITH_INFO) {
    sql_hstmt_ = nullptr;
    return false;
  }
  connected_ = true;
  return true;
}

bool Interface::close() {
  if (sql_hstmt_) {
    try {
      SQLCloseCursor(sql_hstmt_);
      SQLFreeStmt(sql_hstmt_, SQL_UNBIND);
      SQLFreeHandle(SQL_HANDLE_STMT, sql_hstmt_);
      sql_hstmt_ = nullptr;
    }
    catch(...) {
      sql_hstmt_ = nullptr;
    }
  }

  if (sql_hdbc_) {
    try {
      SQLDisconnect(sql_hdbc_);
      SQLFreeHandle(SQL_HANDLE_DBC, sql_hdbc_);
      sql_hdbc_ = nullptr;
    }
    catch(...) {
      sql_hdbc_ = nullptr;
    }
  }

#ifdef MUST_CLOSE_HENV_HANDLE
  if (sql_henv_) {
    try {
      SQLFreeHandle(SQL_HANDLE_ENV, sql_henv_);
      sql_henv_ = nullptr;
    }
    catch(...) {
      sql_henv_ = nullptr;
    }
  }
#endif
  connected_ = false;
  return true;
}

bool Interface::collect_resultinfo() {
  using namespace pf_basic;
  if (!connected_ || !sql_hstmt_) return false;
  if (failed(SQLRowCount(sql_hstmt_, &affect_count_))) return false;
  if (failed(SQLNumResultCols(sql_hstmt_, &column_count_))) return false;
  clear_column_info();
  column_names_ = reinterpret_cast<char **>(
      column_info_allocator_.calloc(sizeof(char *), column_count_ + 1));
  column_values_ = reinterpret_cast<char **>(
      column_info_allocator_.calloc(sizeof(char *), column_count_ + 1)); 
  column_valuelengths_ = reinterpret_cast<SQLINTEGER *>(
      column_info_allocator_.calloc(sizeof(SQLINTEGER), column_count_ + 1));
  column_type_ = reinterpret_cast<SQLSMALLINT *>(
      column_info_allocator_.calloc(sizeof(SQLSMALLINT), column_count_ + 1));
  column_typenames_ = reinterpret_cast<char **>(
      column_info_allocator_.calloc(sizeof(char *), column_count_ + 1));
  column_size_ = reinterpret_cast<SQLULEN *>(
      column_info_allocator_.calloc(sizeof(SQLULEN), column_count_ + 1));
  column_precision_ = reinterpret_cast<SQLSMALLINT *>(
      column_info_allocator_.calloc(sizeof(SQLSMALLINT), column_count_ + 1));
  column_nullable_ = reinterpret_cast<SQLSMALLINT *>(
      column_info_allocator_.calloc(sizeof(SQLSMALLINT), column_count_ + 1));
  for (SQLUSMALLINT column = 0; column < column_count_; ++column) {
    SQLCHAR name[DB_ODBC_COLUMN_NAME_LENGTH_MAX]{0};
    SQLSMALLINT namelength = 0;
    if (failed(SQLDescribeCol(sql_hstmt_, 
                              column + 1, 
                              name, 
                              sizeof(name), 
                              &namelength, 
                              column_type_ + column, 
                              column_size_ + column, 
                              column_precision_ + column, 
                              column_nullable_ + column))) {
      return false;
    }
    name[namelength] = '\0';
    column_names_[column] = reinterpret_cast<char *>(
        column_info_allocator_.calloc(namelength + 1));
    string::safecopy(column_names_[column], 
                     reinterpret_cast<const char *>(name), 
                     namelength + 1);
  }
  return true;
}

bool Interface::execute(const std::string &sql_str) {
  try {
    //int column_index;
    result_ = SQLExecDirect(sql_hstmt_, 
                            cast(SQLCHAR *, sql_str.c_str()), 
                            SQL_NTS);
    if ((result_ != SQL_SUCCESS) && 
        (result_ != SQL_SUCCESS_WITH_INFO) &&
        (result_ != SQL_NO_DATA)) {
      diag_state();
      return false;
    }
    result_ = static_cast<SQLRETURN>(collect_resultinfo());
    return 0 == result_ ? false : true;
  } catch(...) {
    char temp[1024] = {0};
    snprintf(temp, sizeof(temp) - 1, "Huge Error occur: %s", sql_str.c_str());
    save_error_log(temp);
    return false;
  }
  return false;
}

void Interface::clear_no_commit() {
  SQLCloseCursor(sql_hstmt_);
  SQLFreeStmt(sql_hstmt_, SQL_UNBIND);
}

void Interface::clear_column_data() {
  if (column_count_ > 0) {
    for (int32_t column = 0; column < column_count_; ++column) {
      if (column_values_[column] != nullptr) {
        column_values_[column] = nullptr;
      }
    }
  }
  column_value_allocator_.clear();
}

void Interface::clear() {
  SQLCloseCursor(sql_hstmt_);
  SQLFreeStmt(sql_hstmt_, SQL_UNBIND);
  clear_column_info();
  clear_column_data();
}

bool Interface::fetch(int32_t orientation, int32_t offset) {
  clear_column_data();
  SQLRETURN resultcode;
  if (nullptr == sql_hstmt_ || column_count_ < 1) return false;
  if (SQL_FETCH_NEXT == orientation && 0 == offset) {
    resultcode = SQLFetch(sql_hstmt_);
    if (failed(resultcode)) {
      if (resultcode != SQL_NO_DATA) diag_state();
      return false;
    }
  } else {
    resultcode = SQLFetchScroll(sql_hstmt_, 
                                static_cast<SQLSMALLINT>(orientation), 
                                offset);
    if (failed(resultcode)) {
      if (resultcode != SQL_NO_DATA) diag_state();
      return false;
    }
  }
  SQLSMALLINT column;
  for (column = 0; column < column_count_; ++column) {
    char work_data[512] = {0};
    SQLLEN data_length;
    SQLSMALLINT fetchtype = get_typemapping(column_type_[column]);
    if (fetchtype != SQL_C_BINARY) fetchtype = SQL_C_CHAR;
    resultcode = SQLGetData(sql_hstmt_, 
                            column + 1, 
                            fetchtype, 
                            work_data, 
                            sizeof(work_data) - 1, 
                            &data_length);
    if (failed(resultcode)) {
      if (resultcode != SQL_NO_DATA) diag_state();
      return false;
    }
    if (SQL_NULL_DATA == data_length) {
      column_values_[column] = nullptr;
      column_valuelengths_[column] = 0;
    } else if (SQL_SUCCESS_WITH_INFO == resultcode) {
      if (data_length > static_cast<SQLLEN>(sizeof(work_data) - 1)) {
        data_length = static_cast<SQLLEN>(sizeof(work_data) - 1);
        if (SQL_C_CHAR == fetchtype) {
          while((data_length > 1) && (0 == work_data[data_length - 1])) 
            --data_length;
        }
      }
      column_values_[column] = reinterpret_cast<char *>(
          column_value_allocator_.calloc(data_length + 1));
      memcpy(column_values_[column], work_data, data_length);
      column_values_[column][data_length] = '\0';
      column_valuelengths_[column] = static_cast<SQLINTEGER>(data_length);
      for (;;) {
        SQLINTEGER chunklength;
        resultcode = SQLGetData(sql_hstmt_, 
                                static_cast<SQLSMALLINT>(column) + 1, 
                                fetchtype, 
                                work_data, 
                                sizeof(work_data) - 1,
                                &data_length);
        if (SQL_NO_DATA == resultcode) break;
        if (failed(resultcode)) {
          if (resultcode != SQL_NO_DATA) diag_state();
          return false;
        }
        if (data_length > static_cast<SQLLEN>(sizeof(work_data) - 1) && 
            SQL_NO_TOTAL == data_length) {
          chunklength = sizeof(work_data) - 1;
          if (SQL_C_CHAR == fetchtype) {
            while((chunklength > 1) && (work_data[chunklength - 1] == 0)) 
              --chunklength;
          }
        } else {
          chunklength = static_cast<SQLINTEGER>(data_length);
        }
        work_data[chunklength] = '\0';
        column_values_[column] = reinterpret_cast<char *>(
            column_value_allocator_.realloc(
              column_values_[column],
              column_valuelengths_[column] + chunklength + 1));
        memcpy(column_values_[column] + column_valuelengths_[column], 
               work_data, 
               chunklength);
        column_values_[column][data_length] = '\0';
      } //for
    } else {
      column_valuelengths_[column] = static_cast<SQLINTEGER>(data_length);
      column_values_[column] = reinterpret_cast<char *>(
          column_value_allocator_.calloc(data_length + 1));
      memcpy(column_values_[column], work_data, data_length);
      column_values_[column][data_length] = '\0';
    }
    if (SQL_C_CHAR == fetchtype && column_values_[column] != nullptr) {
      char *target = column_values_[column];
      size_t end = strlen(target);
      while (end > 0 && ' ' == target[end - 1]) target[--end] = '\0';
    }
  }
  return true;
}

float Interface::get_float(int32_t column_index, int32_t &error_code) {
  if (column_index > column_count_) {
    error_code = QUERY_NO_COLUMN;
    Assert(false);
    return QUERY_NO_COLUMN;
  }
  if (nullptr == column_values_[column_index]) {
    error_code = QUERY_NULL;
    Assert(false);
    return 0.0f;
  } else {
    error_code = QUERY_OK;
    return static_cast<float>(atof(get_data(column_index)));
  }
  return 0.0f;
}

int64_t Interface::get_int64(int32_t column_index, int32_t &error_code) {
  using namespace pf_basic;
  char temp[128] = {0};
  get_string(column_index, temp, sizeof(temp), error_code);
  int64_t result = string::toint64(temp);
  return result;
}

uint64_t Interface::get_uint64(int32_t column_index, int32_t &error_code) {
  using namespace pf_basic;
  char temp[128] = {0};
  get_string(column_index, temp, sizeof(temp), error_code);
  int64_t result = string::touint64(temp);
  return result;
}

int32_t Interface::get_int32(int32_t column_index, int32_t &error_code) {
  if (column_index > column_count_) {
    error_code = QUERY_NO_COLUMN;
    Assert(false);
    return QUERY_NO_COLUMN;
  }
  if (nullptr == column_values_[column_index]) {
    error_code = QUERY_NULL;
    Assert(false);
    return QUERY_NULL;
  } else {
    error_code = QUERY_OK;
    int32_t result = atoi(get_data(column_index));
    return result;
  }
  return QUERY_NULL;
}

uint32_t Interface::get_uint32(int32_t column_index, int32_t &error_code) {
  if (column_index > column_count_) {
    error_code = QUERY_NO_COLUMN;
    Assert(false);
    return 0;
  }
  if (nullptr == column_values_[column_index]) {
    error_code = QUERY_NULL;
    Assert(false);
    return 0;
  } else {
    error_code = QUERY_OK;
    const char *data = get_data(column_index);
    char *endpointer = nullptr;
    double double_value = strtod(data, &endpointer);
    uint32_t result = static_cast<uint32_t>(double_value);
    return result;
  }
  return 0;
}

int16_t Interface::get_int16(int32_t column_index, int32_t &error_code) {
  int16_t result = static_cast<int16_t>(get_int32(column_index, error_code));
  return result;
}

uint16_t Interface::get_uint16(int32_t column_index, int32_t &error_code) {
  uint16_t result = 
    static_cast<uint16_t>(get_int32(column_index, error_code));
  return result;
}

int8_t Interface::get_int8(int32_t column_index, int32_t &error_code) {
  int8_t result = static_cast<int8_t>(get_int32(column_index, error_code));
  return result;
}

uint8_t Interface::get_uint8(int32_t column_index, int32_t &error_code) {
  uint8_t result = static_cast<uint8_t>(get_int32(column_index, error_code));
  return result;
}

int32_t Interface::get_string(int32_t column_index, 
                              char *buffer, 
                              int32_t buffer_length, 
                              int32_t &error_code) {
  if (column_index >= column_count_) {
    error_code = QUERY_NO_COLUMN;
    buffer[0] = '\0';
    //Assert(false);
    return QUERY_NO_COLUMN;
  }
  if (nullptr == column_values_[column_index]) {
    error_code = QUERY_NULL;
    buffer[0] = '\0';
    //Assert(false);
  } else {
    int32_t data_length = get_datalength(column_index);
    if (data_length <=buffer_length) {
      strncpy(buffer, get_data(column_index), buffer_length);
    } else {
      char message[8092] = {0};
      snprintf(message, 
               sizeof(message) - 1, 
               "buffer_length: %d, data_length: %d, data: %s", 
               buffer_length,
               data_length,
               get_data(column_index));
      AssertEx(false, message);
    }
    error_code = QUERY_OK;
    return data_length;
  }
  return QUERY_NO_COLUMN;
}

int32_t Interface::get_binary(int32_t column_index, 
                              char *buffer, 
                              int32_t buffer_length, 
                              int32_t &error_code) {
  if (column_index >= column_count_) {
    error_code = QUERY_NO_COLUMN;
    buffer[0] = '\0';
    Assert(false);
    return QUERY_NO_COLUMN;
  }
  if (nullptr == column_values_[column_index]) {
    error_code = QUERY_NULL;
    buffer[0] = '\0';
    //Assert(false);
  } else {
    int32_t data_length = get_datalength(column_index);
    if (data_length <= buffer_length) {
      memcpy(buffer, get_data(column_index), buffer_length);
    } else {
      char message[8092] = {0};
      snprintf(message, 
               sizeof(message) - 1, 
               "buffer_length: %d, data_length: %d, data: %s", 
               buffer_length,
               data_length,
               get_data(column_index));
      AssertEx(false, message);
    }
    error_code = QUERY_OK;
    return data_length;
  }
  return QUERY_NO_COLUMN;
}
   
int32_t Interface::get_binary_withdecompress(int32_t column_index, 
                                             char *buffer, 
                                             int32_t buffer_length, 
                                             int32_t &error_code) {
  if (column_index >= column_count_) {
    error_code = QUERY_NO_COLUMN;
    buffer[0] = '\0';
    Assert(false);
    return QUERY_NO_COLUMN;
  }
  if (nullptr == column_values_[column_index]) {
    error_code = QUERY_NULL;
    buffer[0] = '\0';
    //Assert(false);
  } else {
    error_code = QUERY_OK;
    int32_t data_length = get_datalength(column_index);
    if (0 == data_length) return 0;
    if (false == getcompressor()->decompress(
          reinterpret_cast<const unsigned char *>(
            get_data(column_index)), data_length)) {
      Assert(false);
      return 0;
    }
    int32_t decompress_buffersize = 
      static_cast<int32_t>(getcompressor()->get_decompress_buffersize());
    if (decompress_buffersize > buffer_length) {
      char message[8092] = {0};
      snprintf(message, 
               sizeof(message) - 1, 
               "decompress size: %d, realsize: %d", 
               getcompressor()->get_decompress_buffersize(),
               data_length);
      AssertEx(false, message);
    }
    memcpy(buffer, 
           getcompressor()->get_decompress_buffer(), 
           getcompressor()->get_decompress_buffersize());
    int32_t result = getcompressor()->get_decompress_buffersize();
    return result;
  }
  return 0;
}

int32_t Interface::get_field(int32_t column_index, 
                             char *buffer, 
                             int32_t buffer_length, 
                             int32_t &error_code) {
  if (column_index >= column_count_) {
    error_code = QUERY_NO_COLUMN;
    buffer[0] = '\0';
    Assert(false);
    return 0;
  }
  if (nullptr == column_values_[column_index]) {
    error_code = QUERY_NULL;
    buffer[0] = '\0';
    //Assert(false);
  } else {
    uint32_t out_length = 0;
    pf_basic::util::string_tobinary(get_data(column_index), 
                                    get_datalength(column_index), 
                                    buffer, 
                                    buffer_length, 
                                    out_length);
    if (static_cast<int32_t>(out_length) <= buffer_length) {
      error_code = QUERY_OK;
    } else {
      char message[8092] = {0};
      snprintf(message, 
               sizeof(message) - 1,
               "buffer_length: %d, data_length: %d, data: %s",
               buffer_length,
               get_datalength(column_index),
               get_data(column_index));
      AssertEx(false, message);
    }
    return out_length;
  }
  return 0;
}

void Interface::diag_state() {
  int32_t j = 1;
  SQLINTEGER native_error;
  SQLCHAR sql_state[6] = {0};
  SQLSMALLINT msg_length;
  memset(error_message_, 0, ERROR_MESSAGE_LENGTH_MAX);
  while ((result_ = SQLGetDiagRec(SQL_HANDLE_DBC, 
                                  sql_hdbc_,
                                  static_cast<SQLUSMALLINT>(j), 
                                  sql_state,
                                  &native_error,
                                  error_message_, 
                                  sizeof(error_message_), 
                                  &msg_length)) != SQL_NO_DATA) {
    ++j;
  }
  error_message_[ERROR_MESSAGE_LENGTH_MAX - 1] = '\0';
  if (0 == strlen(reinterpret_cast<const char*>(error_message_))) {
    result_ = SQLError(sql_henv_,
                       sql_hdbc_,
                       sql_hstmt_,
                       sql_state,
                       &native_error,
                       error_message_,
                       sizeof(error_message_),
                       &msg_length);
  }
  error_code_ = native_error;
  switch (error_code_) {
    case 2601: { //repeat
      break;
    }
    case 1062: {
      break;
    }
    default: {
      close();
    }
  }
  char error_buffer[2048]{0};
  snprintf(error_buffer,
           sizeof(error_buffer) - 1,
           "error code: %d, error msg: %s", 
           static_cast<int32_t>(error_code_), 
           error_message_);
  save_error_log(error_buffer);
}

void Interface::save_error_log(const char *log) {
  if (0 == strlen(log)) return;
  SLOW_ERRORLOG(DB_MODULENAME, log);
}

void Interface::save_warning_log(const char *log) {
  if (0 == strlen(log)) return;
  SLOW_WARNINGLOG(DB_MODULENAME, log);
}

void Interface::clear_env() {
  if (sql_hstmt_) {
    SQLCloseCursor(sql_hstmt_);
    SQLFreeStmt(sql_hstmt_, SQL_UNBIND);
    SQLFreeHandle(SQL_HANDLE_STMT,sql_hstmt_);
    sql_hstmt_ = nullptr;
  }
  if (sql_hdbc_) {
    SQLDisconnect(sql_hdbc_);
    SQLFreeHandle(SQL_HANDLE_DBC, sql_hdbc_);
    sql_hdbc_ = nullptr;
  }
#ifdef MUST_CLOSE_HENV_HANDLE
  if (sql_henv_) {
    SQLFreeHandle(SQL_HANDLE_ENV, sql_henv_);
    sql_henv_ = nullptr;
  }
#endif
}
//dump field
void Interface::dump(int32_t column_index) {
  char filename[FILENAME_MAX] = {0};
  snprintf(filename, 
           sizeof(filename) - 1,
           "./log/dbfield_%.4d-%.2d-%.2d.log",
           TIME_MANAGER_POINTER->get_year(),
           TIME_MANAGER_POINTER->get_month(),
           TIME_MANAGER_POINTER->get_day());
  FILE *fp = fopen(filename, "a");
  if (fp) {
    fwrite("begin", 1, 5, fp);
    fwrite(get_data(column_index), 
           1, 
           get_datalength(column_index), 
           fp);
    fclose(fp);
  }
}

SQLSMALLINT Interface::get_typemapping(SQLSMALLINT typecode) {
  SQLSMALLINT type = typecode; 
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
      type = SQL_C_CHAR;
      break;
    case SQL_DECIMAL:
      break;
    case SQL_NUMERIC:
      type = SQL_C_NUMERIC;
      break;
    case SQL_SMALLINT:
      type = SQL_C_SSHORT;
      break;
    case SQL_INTEGER:
      type = SQL_C_SLONG;
      break;
    case SQL_REAL:
      type = SQL_C_FLOAT;
      break;
    case SQL_FLOAT:
      break;
    case SQL_DOUBLE:
      type = SQL_C_DOUBLE;
      break;
    case SQL_BIT:
      break;
    case SQL_TINYINT:
      break;
    case SQL_BIGINT:
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
      type = SQL_C_CHAR;
      break;
    case SQL_BINARY:
      break;
    case SQL_VARBINARY:
      break;
    case SQL_LONGVARBINARY:
      type = SQL_C_BINARY;
      break;
    default:
      type = SQL_C_CHAR;
      break;
  }
  return type;
}

const char *Interface::get_data(int32_t column, const char *_default) {
  if (column < 0 || column > column_count_) return _default;
  if (column_values_[column] != nullptr) return column_values_[column];
  return _default;
}

const char *Interface::get_data(const char *columnname, const char *_default) {
  int32_t column = get_index(columnname);
  const char *data = get_data(column, _default);
  return data;
}

int32_t Interface::get_index(const char *columnname) const {
  int32_t result = -1;
  for (int32_t i = 0; i < column_count_; ++i) {
    if (0 == strcmp(columnname, column_names_[i])) {
      result = i;
      break;
    }
  }
  return result;
}

} //namespace odbc

} //namespace pf_db

#endif
