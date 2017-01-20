/**
 * PAP Engine ( https://github.com/viticm/pap )
 * $Id manager.h
 * @link https://github.com/viticm/pap for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm@126.com )
 * @license
 * @user viticm<viticm@126.com>
 * @date 2015/04/06 17:13
 * @uses ai manager class
*/
#ifndef AI_MANAGER_H_
#define AI_MANAGER_H_

#include "ai/config.h"
#include "ai/define.h"
#include "pf/script/lua/interface.h"

namespace ai {

class Manager {

 public:
   Manager();
   virtual ~Manager();

 public:
   static Manager *self_;

 public:
   virtual bool init(const char *filename);
   virtual bool loadconfig(const char *filename);
   virtual bool reloadlogic(const char *filename, int16_t type);
   void set_luainterface(pf_script::lua::Interface *luainterface);
   pf_script::lua::Interface *get_luainterface();
   virtual function_action get_actionfunction(int32_t actionkey) = 0;
   void cleanup();

 public:
   Logic *get(int16_t type);
   Logic *create(int16_t type, int32_t scriptid);

 protected:
   typedef std::map<int16_t, info_t> table_t;
   table_t table_;
   pf_script::lua::Interface *luainterface_;

};

}; //namespace ai

#endif //AI_MANAGER_H_
