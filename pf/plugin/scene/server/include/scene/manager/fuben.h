/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id fuben.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm@126.com>/<viticm.ti@gmail.com>
 * @date 2015/03/19 17:03
 * @uses scene fuben manager class
 *       cn: 副本场景管理器
 *       副本是特殊的场景，它是在附属于场景产生的，当主场景消亡的时候它也会跟着消亡
**/
#ifndef SCENE_MANAGER_FUBEN_H_
#define SCENE_MANAGER_FUBEN_H_

#include "scene/manager/config.h"
#include "pf/sys/thread.h"

namespace scene {

namespace manager {

class PF_PLUGIN_API Fuben {

 public:
   Fuben();
   ~Fuben();

 public:
   typedef struct scene_loadnode_struct {
     scene::Scene *scene;
     scene_loadnode_struct() {
       clear();
     };
     void clear() {
       scene = NULL;
     };
   } scene_loadnode_t;

 public: //以下接口不需要再实现
   bool init();
   sceneid_t selectscene(fuben_select_t *select);
   bool pushscene(scene::Scene *scene);
   scene::Scene *popscene();
   bool addscene(sceneid_t sceneid, scene::Scene *scene);

 public: //子类需要实现的接口
   virtual bool heartbeat(uint32_t time = 0);
   virtual sceneid_t get_bakscene_id() const;
   virtual float get_bakscene_posx() const;
   virtual float get_bakscene_posz() const;
   virtual bool broadcast(); //负载广播

 protected:
   pf_sys::ThreadLock lock_;
   scene_loadnode_t scene_loadnode_[SCENE_MAX];
   uint32_t head_;
   uint32_t tail_;
   uint32_t count_;
   scene::Scene *scenes_[SCENE_MAX];

};

}; //namespace manager

}; //namespace scene

#endif //SCENE_MANAGER_FUBEN_H_
