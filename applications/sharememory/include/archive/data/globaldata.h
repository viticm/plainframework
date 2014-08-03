/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id globaldata.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/08/01 17:30
 * @uses archive data for global data
 */
#ifndef ARCHIVE_DATA_GLOBALDATA_H_
#define ARCHIVE_DATA_GLOBALDATA_H_

#include "archive/data/config.h"
#include "archive/data/interface.h"

namespace archive {

namespace data {

class GlobalData : public Interface {

 public:
   GlobalData(pf_db::Manager *db_manager);
   ~GlobalData();

 public:
   virtual bool load();
   virtual bool erase();
   virtual bool save(void *source);
   virtual bool fetch(void *source);

 public:
   void set_serverid(int32_t id);
   void get_serverid() const;

};

}; //namespace data

}; //namespace archive

#endif //ARCHIVE_DATA_GLOBALDATA_H_
