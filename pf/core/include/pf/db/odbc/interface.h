/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id interface.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.it@gmail.com>
 * @date 2016/06/19 20:24
 * @uses the odbc interface class(some op functions)
 *       version 2.0
 */
#ifndef PF_DB_ODBC_INTERFACE_H_
#define PF_DB_ODBC_INTERFACE_H_

#ifdef PF_OPEN_ODBC

#include "pf/db/odbc/config.h"
#include "pf/sys/memory/dynamic_allocator.h"
#include "pf/util/compressor/mini.h"

//include from odbc
#ifndef VOID
#define VOID void //for unixODBC
#endif

#include "sql.h"
#include "sqlext.h"

namespace pf_db {

namespace odbc {

class PF_API Interface {
 public:
   enum {
     COLUMN_MAX = 100,
     BLOB_BATCH = 10000,
     QUERY_OK = 0,
     QUERY_NULL = -100,
     QUERY_EOF = -101,
     QUERY_NO_COLUMN = -102,
     QUERY_ERROR = -103,
   };

 public:
   Interface();
   ~Interface();
   bool connect(const char *connection_name, 
                const char *user = NULL, 
                const char *password = NULL);
   bool connect();
   bool close();
   bool failed(int32_t code) {
     return SQL_SUCCESS != code && SQL_SUCCESS_WITH_INFO != code;
   };
   bool collect_resultinfo();
   int32_t get_error_code() const { return error_code_; };
   const char *get_error_message() { return (const char *)error_message_; };
   bool is_connected() { return connected_; };
   int32_t get_affect_row_count() { return static_cast<int32_t>(affect_count_); };
   bool is_prepare() { return connected_; };
   void clear();
   void clear_no_commit();
   void clear_column_info() {
     column_info_allocator_.clear();
   };
   void clear_column_data();
   int32_t get_columncount() const { 
     return static_cast<int32_t>(column_count_); 
   };
   bool fetch(int32_t orientation = SQL_FETCH_NEXT, int32_t offset = 0);
   bool execute(const std::string &sql_str);

 public:
   float get_float(int32_t column_index, int32_t &error_code);
   int64_t get_int64(int32_t column_index, int32_t &error_code);
   uint64_t get_uint64(int32_t column_index, int32_t &error_code);
   int32_t get_int32(int32_t column_index, int32_t &error_code);
   uint32_t get_uint32(int32_t column_index, int32_t &error_code);
   int16_t get_int16(int32_t column_index, int32_t &error_code);
   uint16_t get_uint16(int32_t column_index, int32_t &error_code);
   int8_t get_int8(int32_t column_index, int32_t &error_code);
   uint8_t get_uint8(int32_t column_index, int32_t &error_code);
   int32_t get_string(int32_t column_index, 
                      char *buffer, 
                      int32_t buffer_length, 
                      int32_t &error_code);
   int32_t get_field(int32_t column_index, 
                     char *buffer, 
                     int32_t buffer_length, 
                     int32_t &error_code);
   int32_t get_binary(int32_t column_index, 
                      char *buffer, 
                      int32_t buffer_length, 
                      int32_t &error_code);
   int32_t get_binary_withdecompress(int32_t column_index, 
                                     char *buffer, 
                                     int32_t buffer_length, 
                                     int32_t &error_code);

 public:
   const char *get_data(int32_t column_index, const char *_default = NULL);
   const char *get_data(const char *columnname, const char *_default = NULL);
   int32_t get_datalength(int32_t column_index) const {
     if (column_index < 0 || column_index > column_count_) return 0;
     return column_valuelengths_[column_index];
   };
   int32_t get_index(const char *columnname) const;
   int16_t get_size(int32_t column_index) const {
     if (column_index < 0 || column_index > column_count_) return -1;
     int16_t result = static_cast<int16_t>(column_size_[column_index]);
     return result;
   };
   int16_t get_precision(int32_t column_index) const {
     if (column_index < 0 || column_index > column_count_) return -1;
     return column_precision_[column_index];
   };
   int16_t get_nullable(int32_t column_index) const {
     if (column_index < 0 || column_index > column_count_) return -1;
     return column_nullable_[column_index];
   };
   const char *get_name(int32_t column_index) {
     if (column_index < 0 || column_index > column_count_) return nullptr;
     return column_names_[column_index];
   };
   int16_t get_type(int32_t column_index) const {
     if (column_index < 0 || column_index > column_count_) return -1;
     return column_type_[column_index];
   };
   const char *get_typename(int16_t column_index) {
     if (column_index < 0 || column_index > column_count_) return nullptr;
     return column_typenames_[column_index];
   };
   pf_util::compressor::Mini *getcompressor() {
     return &compressor_;
   };
   const char *get_connection_name() { return connection_name_; }

   //分析
   void diag_state();
   void save_error_log(const char *log);
   void save_warning_log(const char *log);
   void clear_env();
   void dump(int32_t column_index);

 private:
   SQLSMALLINT get_typemapping(SQLSMALLINT typecode);

 private:
   bool connected_;
   SQLHENV sql_henv_;
   SQLHDBC sql_hdbc_;
   SQLRETURN result_;
   char connection_name_[DB_CONNECTION_NAME_LENGTH];
   char user_[DB_USER_NAME_LENGTH];
   char password_[DB_PASSWORD_LENGTH];

   SQLLEN affect_count_;
   SQLHSTMT sql_hstmt_;
   
   /* column about { */
   SQLSMALLINT column_count_;
   char **column_names_;
   SQLSMALLINT *column_type_;
   char **column_typenames_;
   SQLULEN *column_size_;
   SQLSMALLINT *column_precision_;
   SQLSMALLINT *column_nullable_;
   char **column_values_;
   SQLINTEGER *column_valuelengths_;
   pf_sys::memory::DynamicAllocator column_info_allocator_;
   pf_sys::memory::DynamicAllocator column_value_allocator_;
   /* column about } */

   pf_util::compressor::Mini compressor_;

   SQLINTEGER error_code_;
   SQLCHAR error_message_[ERROR_MESSAGE_LENGTH_MAX];

};

}; //namespace odbc

}; //namespace pf_db

#endif //PF_OPEN_ODBC

#endif //PF_DB_ODBC_INTERFACE_H_
