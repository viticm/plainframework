/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id nodelogic_manager.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/07/31 15:41
 * @uses archive node logic manager class
 */
#ifndef ARCHIVE_NODELOGIC_MANAGER_H_
#define ARCHIVE_NODELOGIC_MANAGER_H_

#include "archive/config.h"
#include "pf/base/singleton.h"

namespace archive {

class NodeLogicManager : public pf_base::Singleton<NodeLogicManager> {

 public:
   NodeLogicManager();
   ~NodeLogicManager();

 public:
   static NodeLogicManager *getsingleton_pointer();
   static NodeLogicManager &getsingleton();

 public:
   void setnode(int32_t index, void *node);
   void *getnode(int32_t index);
   void clear();
   void clear(uint64_t key);
   bool allocate();
   bool init();
   bool release();

 private:
   void *logicdata_[ARCHIVE_OBJMAX];

};

}; //namespace archive

extern archive::NodeLogicManager *g_archive_nodelogic_manager;

#define ARCHIVE_NODELOGIC_MANAGER_POINTER \
  archive::NodeLogicManager::getsingleton_pointer()

#endif //ARCHIVE_NODELOGIC_MANAGER_H_
