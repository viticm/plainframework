/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id interface.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/08/01 18:53
 * @uses the archive data interface class
 */
#ifndef ARCHIVE_DATA_INTERFACE_H_
#define ARCHIVE_DATA_INTERFACE_H_

#include "archive/data/config.h"
#include "pf/db/manager.h"

namespace archive {

namespace data {

class Interface {

 public:
   Interface();
   virtual ~Interface() {};

 public:
   typedef enum {
     kDBOptionTypeLoad,
     kDBOptionTypeSave,
     kDBOptionTypeAdd,
     kDBOptionTypeErase,
   } db_optiontype_t;
   typedef enum {
     kDBErrorSamePrimaryKey = -2,
   } db_error_t;

 public:
   virtual bool load();
   virtual bool add();
   virtual bool erase();
   virtual bool save(void *source);
   virtual bool fetch(void *source) = 0;

 protected:
   db_optiontype_t db_optiontype_;
   pf_db::Manager *db_manager_;

};

}; //namespace data

}; //namespace archive

#endif //ARCHIVE_DATA_INTERFACE_H_
