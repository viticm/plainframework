/**
 * PAP Engine ( https://github.com/viticm/pap )
 * $Id state.h
 * @link https://github.com/viticm/pap for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm@126.com )
 * @license
 * @user viticm<viticm@126.com>
 * @date 2015/04/04 15:53
 * @uses ai state class
 *       cn: AI状态类，状态->事件->动作
*/
#ifndef AI_STATE_H_
#define AI_STATE_H_

#include "ai/config.h"
#include "ai/define.h"

namespace ai {

class State {

 public:
   State(Logic *logic);
   virtual ~State();

 public:
   void setlogic(Logic *logic);
   action_t get_eventhandle(int32_t eventid);
   void add_eventhandle(int32_t eventid, int32_t actionid);

 protected:
   std::vector<eventhandle_t> eventhandles_;
   Logic *logic_;

};

}; //namespace ai

#endif //AI_STATE_H_
