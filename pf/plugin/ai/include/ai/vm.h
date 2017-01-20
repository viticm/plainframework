/**
 * PAP Engine ( https://github.com/viticm/pap )
 * $Id vm.h
 * @link https://github.com/viticm/pap for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm@126.com )
 * @license
 * @user viticm<viticm@126.com>
 * @date 2015/04/06 11:52
 * @uses ai vm class
*/
#ifndef AI_VM_H_
#define AI_VM_H_

#include "ai/config.h"
#include "pf/base/tinytimer.h"
#include "structure/define.h"

namespace ai {

class VM {

 public:
   VM();
   ~VM();

 public:
   bool init(structure::Object *object, int8_t type);
   void heartbeat(uint32_t time = 0);
   void cleanup();

 public:
   int8_t gettype() const;
   void fireevent(int32_t eventid, int32_t eventsource, int32_t eventparam);
   void setstate(int16_t state);
   void set_primarytimer(int32_t frame);
   void set_secondarytimer(int32_t frame);
   void set_tertiarytimer(int32_t frame);

 public:
   uint64_t runtime_statistic_;

 protected:
   structure::Object *self_;
   int8_t type_;
   Logic *logic_;
   State *state_;
   int32_t stateid_;
   int32_t actionid_;
   pf_base::TinyTimer primarytimer_; //第一定时器
   pf_base::TinyTimer secondarytimer_; //第二定时器
   pf_base::TinyTimer tertiarytimer_; //第三定时器
   int32_t current_eventid_;
   int32_t pending_eventid_;
   int32_t pending_eventsource_;
   int32_t pending_eventparam_;

};

}; //namespace ai

#endif //AI_VM_H_
