/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id objectsingle.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm@126.com>/<viticm.ti@gmail.com>
 * @date 2015/03/18 18:00
 * @uses structure object single manager class
 *       cn: 对象单管理器
**/
#ifndef STRUCTURE_MANAGER_OBJECTSINGLE_H_
#define STRUCTURE_MANAGER_OBJECTSINGLE_H_

#include "structure/manager/config.h"
#include "pf/base/tinytimer.h"

namespace structure {

namespace manager {

class PF_PLUGIN_API ObjectSingle {

 public:
   ObjectSingle();
   virtual ~ObjectSingle();

 public:
   virtual bool init(objectsingle_manager_init_t *initdata);
   virtual void destroy();
   void reset();
   void set_loadfactor(float factor);
   virtual bool heartbeat(uint32_t time = 0);
   bool add(structure::Object *object);
   bool remove(structure::Object *object);
   uint32_t getcount() const;
   uint32_t getlength() const;
   void setscene(void *scene);
   void *getscene();
   structure::Object *get(uint32_t index);
 
 private: 
   bool resize(uint32_t size);

 private:
   structure::Object **objects_;
   uint32_t count_;
   uint32_t length_;
   uint32_t logicinterval_;
   uint32_t current_logicinterval_;
   void *scene_;
   pf_base::TinyTimer logictimer_;
};

}; //namespace manager

}; //namespace structure

#endif //STRUCTURE_MANAGER_OBJECTSINGLE_H_
