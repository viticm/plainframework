/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id net.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm@126.com>/<viticm.ti@gmail.com>
 * @date 2015/03/18 17:04
 * @uses scene net manager class
 *       cn: 场景网络管理器，该管理器主要是管理网络连接，与普通的网络管理器不同
**/
#ifndef SCENE_MANAGER_NET_H_
#define SCENE_MANAGER_NET_H_

#include "scene/manager/config.h"
#include "pf/net/manager.h"

namespace scene {

namespace manager {

class PF_PLUGIN_API Net : public pf_net::Manager {

 public:
   Net();
   virtual ~Net();

 public:
   sceneid_t get_sceneid() const;
   void set_sceneid(sceneid_t sceneid);
   virtual bool process_cachecommand();
   virtual bool sendpacket(pf_net::packet::Base *packet, 
                           int16_t serverid, 
                           uint32_t flag = kPacketFlagNone);
   void broadcast(pf_net::packet::Base *packet, 
                  scene::ChatPipeline *chatpipeline = NULL, 
                  int32_t status = -1);

 private:
   sceneid_t sceneid_;

};

}; //namespace manager

}; //namespace scene;

#endif //SCENE_MANAGER_NET_H_
