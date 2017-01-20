/**
 * PAP Engine ( https://github.com/viticm/pap )
 * $Id base.h
 * @link https://github.com/viticm/pap for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm@126.com )
 * @license
 * @user viticm<viticm@126.com>
 * @date 2015/04/06 12:07
 * @uses ai base class
*/
#ifndef AI_BASE_H_
#define AI_BASE_H_

#include "ai/config.h"
#include "structure/define.h"
#include "ai/define.h"

namespace ai {

class Base {

 public:
   Base();
   virtual ~Base();

 public:
   virtual bool init(structure::Object *self);
   virtual void cleanup();

 public:
   bool isinit() const;
   int8_t gettype() const;
   void settype(int8_t type);
   void setinterval(int32_t interval);
   virtual void setinterval(float rate);
   void set_mainstate(int16_t state);
   int16_t get_mainstate() const;
   int16_t getstate() const;
   structure::position_t *get_origin_position();
   void set_origin_position(structure::position_t *position);
   void setparam(int32_t index, int32_t value);

 public:
   virtual bool heartbeat(uint32_t time = 0);
   virtual void checkevent(uint32_t time = 0); 
   virtual void fireevent(int16_t eventcondition_id);

 protected:
   bool isinit_;
   int8_t type_; //类型
   int16_t state_; //状态
   int16_t mainstate_; //不可变状态
   structure::position_t origin_position_; //初始位置
   int32_t eventtimer_[AI_EVENT_MAX]; //事件列表
   int32_t params_[AI_PARAM_COUNT]; //参数列表
   int32_t interval_; //执行间隔
   structure::Object *self_; //挂载AI的对象指针
   Template *template_; //AI模板数据

 private:
   friend class Controller;
   friend class Event;

};

}; //namespace ai

#endif //AI_BASE_H_
