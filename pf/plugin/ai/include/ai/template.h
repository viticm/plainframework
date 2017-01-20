/**
 * PAP Engine ( https://github.com/viticm/pap )
 * $Id template.h
 * @link https://github.com/viticm/pap for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm@126.com )
 * @license
 * @user viticm<viticm@126.com>
 * @date 2015/04/07 12:04
 * @uses ai template class
 */
#ifndef AI_TEMPLATE_H_
#define AI_TEMPLATE_H_

#include "ai/config.h"
#include "ai/event.h"

namespace ai {

class Template {

 public:
   Template();
   ~Template();

 public:
   Event events_[AI_EVENT_MAX];

};

}; //namespace ai

#endif //AI_TEMPLATE_H_
