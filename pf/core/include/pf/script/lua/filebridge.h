/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id filebridge.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2017/01/11 16:51
 * @uses your description
*/
#ifndef PF_SCRIPT_LUA_FILEBRIDGE_H_
#define PF_SCRIPT_LUA_FILEBRIDGE_H_

#include "pf/script/lua/config.h"
#include "pf/basic/string.h"

#ifdef PF_OPEN_LUA

namespace pf_script {

namespace lua {

class PF_API FileBridge { 

 public:
   FileBridge();
   ~FileBridge() { close(); };

 public:
   typedef enum {
     kFileAccessModeBegin = 0,
     kFileAccessModeCurrent,
     kFileAccessModeEnd
   } file_accessmode_t;

 public:
   void set_rootpath(const std::string &path) {
     pf_basic::string::safecopy(rootpath_, path.c_str(), sizeof(rootpath_));
   };
   void set_workpath(const std::string &path) {
     pf_basic::string::safecopy(workpath_, path.c_str(), sizeof(workpath_));
   };
   const char *get_rootpath() {
     return rootpath_;
   }
   const char *get_workpath() {
     return workpath_;
   }

 public:
   bool open(const char *);
   void close();
   uint64_t read(void *, uint64_t);
   uint64_t write(void *, uint64_t);
   int64_t seek(int64_t, file_accessmode_t);
   int64_t tell() {
     if (!fp_) return -1;
     return position_;
   };
   uint64_t size();

 public:
   void get_fullpath(char *path, const char *filename, size_t length);

 private:
   FILE *fp_;
   uint64_t length_;
   int64_t position_;
   char rootpath_[FILENAME_MAX];
   char workpath_[FILENAME_MAX];

};

}; //namespace lua

}; //namespace pf_script

#endif //PF_OPEN_LUA

#endif //PF_SCRIPT_LUA_FILEBRIDGE_H_
