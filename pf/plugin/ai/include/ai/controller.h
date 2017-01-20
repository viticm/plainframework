/**
 * PAP Engine ( https://github.com/viticm/pap )
 * $Id control.h
 * @link https://github.com/viticm/pap for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm@126.com )
 * @license
 * @user viticm<viticm@126.com>
 * @date 2015/04/06 11:50
 * @uses ai control class
*/
#ifndef AI_CONTROLLER_H_
#define AI_CONTROLLER_H_

#include "ai/config.h"
#include "structure/define.h"
#include "ai/base.h"

namespace ai {

class Controller {

 public:
   Controller();
   virtual ~Controller();

 public:
   bool init(int8_t type, structure::Object *self);
   void cleanup();

 public:
   bool heartbeat(uint32_t time = 0);
   void stop();
   virtual void restart();
   bool isrunning() const;
   virtual bool set_aitype(int8_t type);
   int32_t get_event_nextframe(int32_t eventindex);
   Base *getai();

 protected:
   Base *base_;
   unsigned char base_memory_[sizeof(Base)];
   bool isrunning_;

};

}; //namespace ai

#endif //AI_CONTROLLER_H_
