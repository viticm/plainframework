/**
 * PAP Engine ( https://github.com/viticm/pap )
 * $Id define.h
 * @link https://github.com/viticm/pap for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm@126.com )
 * @license
 * @user viticm<viticm@126.com>
 * @date 2015/04/04 15:52
 * @uses ai define file
*/
#ifndef AI_DEFINE_H_
#define AI_DEFINE_H_

#include "ai/config.h"
#include "structure/define.h"

#define AI_PARAM_COUNT 10
#define AI_ACTION_BRANCH_COUNT 6
#define AI_EVENT_MAX 10
#define AI_MODULENAME "AI"
#define AI_USERACTION_START 1000 //用户自己的动作开始ID

namespace ai {

/* enum { */
enum {
  kTypeInvalid = -1,
  kTypeBase,
  kTypeHuman,
  kTypeMonster,
  kTypeWood,
  kTypeNumber
}; //类型枚举

enum {
  kEventInvalid = ID_INVALID,
  kEventPrimaryTimer,
  kEventSecondaryTimer,
  kEventTertiaryTimer,
  kEventNumber
}; //事件枚举
/* } enum */

typedef int32_t (__stdcall *function_action)(structure::Object*, Action *);

/* struct { */
typedef struct action_struct action_t;
struct action_struct {
  int16_t id;
  Action *action;
  action_struct();
  action_t &operator = (action_t const &object);
}; //动作结构

typedef struct eventcondition_struct eventcondition_t;
struct eventcondition_struct {
  int16_t id;
  int32_t value;
  eventcondition_struct();
}; //事件条件结构

typedef struct info_struct info_t;
struct info_struct {
  int32_t scriptid;
  Logic *logic;
  info_struct();
}; //基本信息结构

typedef struct branch_struct branch_t;
struct branch_struct {
  int16_t id;
  Action *action;
  branch_struct();
};

typedef struct eventhandle_struct eventhandle_t;
struct eventhandle_struct {
  int32_t eventid;
  action_t actioninfo;
  eventhandle_struct();
};
/* } struct */

}; //namespace ai

#endif //AI_DEFINE_H_
