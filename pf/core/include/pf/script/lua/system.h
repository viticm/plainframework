/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id system.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2017/01/11 19:32
 * @uses The lua system module.
*/
#ifndef PF_SCRIPT_LUA_SYSTEM_H_
#define PF_SCRIPT_LUA_SYSTEM_H_

#ifdef PF_OPEN_LUA

#include <lua.hpp>
#include "pf/script/lua/config.h"
#include "pf/script/interface.h"
#include "pf/script/lua/filebridge.h"

namespace pf_script {

namespace lua {

class PF_API System : public Interface {

 public:
   System() : lua_state_{nullptr}, stack_size_{0} {};
   virtual ~System() { release(); };

 public:
   typedef enum {
     kErrorCodeCreate = 1,
     kErrorCodeLength = 2,
     kErrorCodeLoadBuffer = 3,
     kErrorCodeExecute = 4,
     kErrorCodeNotNumber = 5,
     kErrorCodeNotString = 6,
     kErrorCodeNotTable = 7,
     kErrorCodeStateIsNil = 8,
     kErrorCodeResize = 9,
   } errorcode_t;

 public:
   virtual bool init();
   virtual bool bootstrap(const std::string &filename);
   virtual void release();

 public:
   virtual bool load(const std::string &filename);
   virtual bool reload(const std::string &filename) {
     std::unique_lock<std::mutex> autolock(mutex_);
     unregister_refs();
     return load(filename);
   };

 public: //env.
   virtual void set_rootpath(const std::string &path) {
     filebridge_.set_rootpath(path);
   };
   virtual void set_workpath(const std::string &path) {
     filebridge_.set_workpath(path);
   };

 public:
   virtual void register_function(const std::string &name, void *pointer) {
     if (is_null(lua_state_)) return;
     lua_register(
         lua_state_, name.c_str(), reinterpret_cast< lua_CFunction >(pointer));
   };
   virtual void setglobal(const std::string &name,
                          const pf_basic::type::variable_t &var);
   virtual void getglobal(const std::string &name, 
                          pf_basic::type::variable_t &var);
   virtual bool exists(const std::string &name) {
     lua_getglobal(lua_state_, name.c_str());
     return lua_isnil(lua_state_, 1) != 1;
   };
   //Call with string, example: "test\t1\tname" or "module.test\t1\tname".
   virtual bool call(const std::string &str);
   virtual bool call(const std::string &name, 
                     const pf_basic::type::variable_array_t &params,
                     pf_basic::type::variable_array_t &results);

 public: //For lua owner.
   lua_State *get_lua_state() {
     return lua_state_;
   };
   bool loadstring(const std::string &str) {
      if (is_null(lua_state_)) return false;
      luaL_loadstring(lua_state_, str.c_str());
      return true;
   };
   bool loadbuffer(const char *buffer, size_t size);
   bool register_table(const char *name, const struct luaL_Reg regtable[]);
   void set_stack_size(size_t size) { stack_size_ = size; };
   bool register_ref(const std::string &table, const std::string &field); 
   bool get_ref(const std::string &table, const std::string &field);
   bool unregister_ref(const std::string &table, const std::string &field);
   void unregister_refs();
   void gccheck(int32_t freetime);
   void setfield(
       const std::string &table, const std::string &field, const pf_basic::type::variable_t &var);
   void getfield(const std::string &table, const std::string &field, pf_basic::type::variable_t &var);
   bool callstr(const std::string &str) {
     return 1 == luaL_dostring(lua_state_, str.c_str());
   };

 private:
   void callenter(int32_t &index) {
     if (is_null(lua_state_)) return;
     index = lua_gettop(lua_state_);
   };
   void callleave(int32_t index) {
     if (is_null(lua_state_)) return;
     lua_settop(lua_state_, index);
   };
   bool executecode();
   void open_libs();
   void on_scripterror(int32_t error);
   void on_scripterror(int32_t error1, int32_t error2);

 private:
   lua_State *lua_state_;
   FileBridge filebridge_;
   std::map< std::string, int32_t > refs_;
   size_t stack_size_;
   std::mutex mutex_;

};

}; //namespace lua

}; //namespace pf_script

#endif //PF_OPEN_LUA

#endif //PF_SCRIPT_LUA_SYSTEM_H_
