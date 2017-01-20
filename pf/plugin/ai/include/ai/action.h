/**
 * PAP Engine ( https://github.com/viticm/pap )
 * $Id action.h
 * @link https://github.com/viticm/pap for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm@126.com )
 * @license
 * @user viticm<viticm@126.com>
 * @date 2015/04/04 15:42
 * @uses ai action class
*/
#ifndef AI_ACTION_H_
#define AI_ACTION_H_

#include "ai/config.h"
#include "ai/define.h"

namespace ai {

class Action {

 public:
   Action();
   ~Action();

 public:
   branch_t branchs_[AI_ACTION_BRANCH_COUNT];
   int32_t params_[AI_PARAM_COUNT];
   int32_t key_;

 public:
   void setbranch(int16_t index, branch_t *branch);
   void setparam(int8_t index, int32_t value);

};

}; //namespace ai

#endif //AI_ACTION_H_
