/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id object.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm@126.com>/<viticm.ti@gmail.com>
 * @date 2015/03/18 17:20
 * @uses structure object manager
 *       cn: 基本的对象管理器
**/
#ifndef STRUCTURE_MANAGER_OBJECT_H_
#define STRUCTURE_MANAGER_OBJECT_H_

#include "structure/manager/config.h"

#if __WINDOWS__
#define STRUCTURE_MANAGER_OBJECT_IDSIZE_MAX 5000
#define STRUCTURE_MANAGER_OBJECT_HUMAN_START 2500
#elif __LINUX__
#define STRUCTURE_MANAGER_OBJECT_IDSIZE_MAX 30000
#define STRUCTURE_MANAGER_OBJECT_HUMAN_START 15000
#endif

namespace structure {

namespace manager {

class PF_PLUGIN_API Object {

 public:
   Object();
   virtual ~Object();

 public:
   bool init(const objectmanager_init_t *initdata);
   void destroy();

 public:
   bool add(structure::Object *object, int8_t idtype);
   bool remove(objectid_t id);
   structure::Object *get(objectid_t id);
   int32_t get_normalcount() const;
   int32_t get_humancount() const;
   object_idtype_t get_idtype(objectid_t id) const;

 private:
   structure::Object *objects_[STRUCTURE_MANAGER_OBJECT_IDSIZE_MAX];
   int32_t normalposition_;
   int32_t humanposition_;
};

}; //namespace manager

}; //namespace structure

#endif //STRUCTURE_MANAGER_OBJECT_H_
