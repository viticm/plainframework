/**
 * PAP Engine ( https://github.com/viticm/pap )
 * $Id event.h
 * @link https://github.com/viticm/pap for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm@126.com )
 * @license
 * @user viticm<viticm@126.com>
 * @date 2015/04/06 11:51
 * @uses ai event class
*/
#ifndef AI_EVENT_H_
#define AI_EVENT_H_

#include "ai/config.h"
#include "structure/define.h"
#include "ai/define.h"

namespace ai {

class Event {

 public:
   Event();
   virtual ~Event();

 public:
   virtual void setaction(int16_t id);
   virtual bool runaction(structure::Object *self);
   bool init(int16_t actionid, bool conditionlogic, bool runonce);
   void set_cdframe(int32_t cdframe);
   int32_t get_cdframe() const;
   bool addcondition(int16_t conditionid, int32_t value);
   virtual bool checkcondition(structure::Object *self, 
                               int32_t eventindex, 
                               int32_t current);
   bool checkevent(structure::Object *self, 
                   int32_t eventindex, 
                   int32_t current = ID_INVALID);
   bool fireevent(structure::Object *self, int32_t eventindex, int32_t current);
   bool runevent(structure::Object *self);
   bool is_runonce() const;

 protected:
   std::vector<eventcondition_t> eventcondition_table_; //事件条件容器
   bool conditionlogic_; //条件或还是与 true 表示或 false 表示与
   bool runonce_; //是否只执行一次，不是则在CD之后可以被重复触发
   int32_t cdframe_; //CD间隔
   int16_t actionid_; //动作ID
   int32_t scriptid_; //脚本ID
   bool isactive_; //是否激活

};

}; //namespace ai

#endif //AI_EVENT_H_
