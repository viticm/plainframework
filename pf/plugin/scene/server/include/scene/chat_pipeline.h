/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id chat_pipeline.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm@126.com>/viticm.ti@gmail.com
 * @date 2015/03/16 14:54
 * @uses server scene chat pipe class
 *       cn: 服务器场景聊天通道类
 *       聊天非及时发送，会有一段缓冲时间，负责场景中玩家聊天的信息管理
*/
#ifndef SCENE_CHAT_PIPELINE_H_
#define SCENE_CHAT_PIPELINE_H_

#include "scene/config.h"
#include "scene/define.h"
#include "pf/net/packet/base.h"
#include "pf/sys/thread.h"

#define SCENE_CHAT_ITEM_SIZE 1024
#define SCENE_CHAT_COUNTSCHAT_PERTICK 128
#define SCENE_CHAT_TIPS_SIZE 128
#define SCENE_CHAT_COUNTSTIPS_PRETICK 16

namespace scene {

class PF_PLUGIN_API ChatPipeline {

 public:
   ChatPipeline();
   ~ChatPipeline();

 public:
   void clear();
   bool init(Scene *scene);
   virtual bool heartbeat(uint32_t time);
   Scene *getscene();
   void dec_validcount(int32_t count = 1);

 public:
   bool packet_send(pf_net::packet::Base *packet, 
                    objectid_t sourceid, 
                    objectid_t targetid, 
                    int32_t type);
   bool packet_chatrecv(pf_net::packet::Base *&packet, 
                        objectid_t &sourceid, 
                        objectid_t &targetid);
   bool packet_tipsrecv(pf_net::packet::Base *&packet,
                        objectid_t &sourceid,
                        objectid_t &targetid);

 private:
   int32_t validcount_;
   Scene *scene_;
   chatitem_t *chatitems_;
   int32_t chathead_;
   int32_t chattail_;
   chatitem_t *tipsitems_;
   int32_t tipshead_;
   int32_t tipstail_;
   pf_sys::ThreadLock lock_;

};

}; //namespace scene

#endif //SCENE_CHAT_PIPELINE_H_
