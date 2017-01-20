/**
 * PAP Engine ( https://github.com/viticm/pap )
 * $Id logic.h
 * @link https://github.com/viticm/pap for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm@126.com )
 * @license
 * @user viticm<viticm@126.com>
 * @date 2015/04/06 12:12
 * @uses ai logic class
*/
#ifndef AI_LOGIC_H_
#define AI_LOGIC_H_

#include "ai/config.h"
#include "ai/define.h"

namespace ai {

class Logic {

 public:
   Logic();
   virtual ~Logic();

 public:
   typedef std::map<int32_t, Action> actiontable_t;
   typedef std::map<int32_t, std::string> user_actiontable_t;
   typedef std::map<int32_t, State> statetable_t;

 public:
   virtual bool init(int8_t type, int32_t scriptid = ID_INVALID);

 public:
   Action *getaction(int16_t id);
   State *getstate(int16_t id);
   int16_t get_initstate() const;
   int32_t get_scriptid() const;
   action_t callaction(structure::Object *object, action_t actiondata);
   void newaction(int32_t actionid, int32_t actionkey);
   void newstate(int16_t state);
   void set_initstate(int16_t state);
   void register_useraction(int32_t key, const char *functionname);

 protected:
   actiontable_t actiontable_;
   user_actiontable_t user_actiontable_;
   statetable_t statetable_;
   int8_t type_;
   int32_t scriptid_;
   int16_t initstate_;

};

}; //namespace ai

#endif //AI_LOGIC_H_
