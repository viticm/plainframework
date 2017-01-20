/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id config.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm@126.com>/viticm.ti@gmail.com
 * @date 2015/03/17 14:40
 * @uses structure config file
 *       cn: 数据结构配置文件
*/
#ifndef STRUCTURE_CONFIG_H_
#define STRUCTURE_CONFIG_H_

#include "pf/base/config.h"

namespace structure {

class Object;
class ObjectList;

namespace manager {
class Object;
class ObjectSingle;
}; //namespace manager

typedef struct position_struct position_t;
typedef struct object_struct object_t;
typedef struct objectlist_node_struct objectlist_node_t;
typedef struct campdata_struct campdata_t;
typedef struct objectlist_struct objectlist_t;

}; //namespace structure

#endif //STRUCTURE_CONFIG_H_
