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
#ifndef ARCHIVE_NODE_LOGIC_MANAGER_H_
#define ARCHIVE_NODE_LOGIC_MANAGER_H_

#include "archive/node/config.h"
#include "pf/base/singleton.h"

namespace archive {

namespace node {

class LogicManager : public pf_base::Singleton<LogicManager> {

 public:
   LogicManager();
   ~LogicManager();

 public:
   static LogicManager *getsingleton_pointer();
   static LogicManager &getsingleton();

 public:
   void setnode(int32_t index, void *node);
   void *getnode(int32_t index);
   void clearlog();
   void createlog(uint32_t key);
   bool allocate();
   bool init();
   bool tick();
   bool release();

 private:
   void *logicdata_[ARCHIVE_OBJMAX];

};

}; //namespace node

}; //namespace archive

extern archive::node::LogicManager *g_archive_nodelogic_manager;

#define ARCHIVE_NODELOGIC_MANAGER_POINTER \
  archive::node::LogicManager::getsingleton_pointer()

#endif //ARCHIVE_NODE_LOGIC_MANAGER_H_
