/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id recycleconnection.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm@126.com>/<viticm.ti@gmail.com>
 * @date 2015/03/19 11:44
 * @uses scene recycle connection manager
 *       cn: 场景循环使用的连接管理器
 *       玩家连接的回收器，连接通过该管理器回收并继续使用
**/
#ifndef SCENE_MANAGER_RECYCLECONNECTION_H_
#define SCENE_MANAGER_RECYCLECONNECTION_H_

#include "scene/manager/config.h"
#include "pf/net/manager.h"

namespace scene {

namespace manager {

class PF_PLUGIN_API RecycleConnection : public pf_net::Manager {

 public:
   RecycleConnection();
   virtual ~RecycleConnection();

 public:
   virtual bool heartbeat(uint32_t time = 0);

 public:
   void set_sceneid(sceneid_t sceneid);
   sceneid_t get_sceneid() const;

 private:
   sceneid_t sceneid_;

};

}; //namespace manager

}; //namespace scene

#endif //SCENE_MANAGER_RECYCLECONNECTION_H_
