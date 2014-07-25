/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id factorymanager.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.it@gmail.com>
 * @date 2014/06/22 14:16
 * @uses net packet factory manager
 */
#ifndef PF_NET_PACKET_FACTORYMANAGER_H_
#define PF_NET_PACKET_FACTORYMANAGER_H_

#include "pf/sys/thread.h"
#include "pf/base/singleton.h"
#include "pf/net/config.h"
#include "pf/net/packet/factory.h"
#include "pf/base/hashmap/template.h"

namespace pf_net {

namespace packet {

class FactoryManager : public pf_base::Singleton<FactoryManager> {

 public:
   FactoryManager();
   ~FactoryManager();

 public:
   uint32_t* packet_alloccount_;

 public:
   static FactoryManager& getsingleton();
   static FactoryManager* getsingleton_pointer();
 
 public:
   bool init();
   //根据消息类型从内存里分配消息实体数据（允许多线程同时调用）
   Base* createpacket(uint16_t pakcetid);
   //根据消息类型取得对应消息的最大尺寸（允许多线程同时调用）
   uint32_t getpacket_maxsize(uint16_t packetid);
   //删除消息实体（允许多线程同时调用）
   void removepacket(Base* packet);
   void lock();
   void unlock();
   static bool isvalid_packetid(uint16_t id); //packetid is valid

 private:
   Factory** factories_;
   pf_base::hashmap::Template<uint16_t, uint16_t> idindexs_;
   uint16_t size_;
   uint16_t factorycount_;
   pf_sys::ThreadLock lock_;

 private:
   void addfactory(Factory* factory);
   void extend_forconstruct(); //构造的扩展函数，对象建立的时候即调用
   void extend_forinit(); //初始化的扩展函数，调用函数init初始化时调用
   static bool extend_for_packetid_isvalid(uint16_t id); //扩展包ID是否有效

};

}; //namespace packet

}; //namespace pap_common_net

extern pf_net::packet::FactoryManager* g_packetfactory_manager;

#define NET_PACKET_FACTORYMANAGER_POINTER \
  pf_net::packet::FactoryManager::getsingleton_pointer()

#endif //PF_NET_PACKETFACTORY_H_
