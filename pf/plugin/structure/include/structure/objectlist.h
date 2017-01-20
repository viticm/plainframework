/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id objectlist.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm@126.com>/<viticm.ti@gmail.com>
 * @date 2015/03/18 10:45
 * @uses structure object list class
 *       cn: 对象列表类
**/
#ifndef STRUCTURE_OBJECTLIST_H_
#define STRUCTURE_OBJECTLIST_H_

#include "structure/config.h"
#include "structure/define.h"

namespace structure {

class PF_PLUGIN_API ObjectList {

 public:
   ObjectList();
   virtual ~ObjectList();

 public:
   objectlist_node_t head_;
   objectlist_node_t tail_;

 public:
   void cleanup();
   bool addnode(objectlist_node_t *node);
   bool removenode(objectlist_node_t *node);
   int32_t getsize() const;

 private:
   int32_t size_;

};

}; //namespace structure

#endif //STRUCTURE_OBJECTLIST_H_
