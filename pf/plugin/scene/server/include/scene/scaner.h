/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id scaner.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm@126.com>/<viticm.ti@gmail.com>
 * @date 2015/03/19 12:05
 * @uses scene scaner class
 *       cn: 场景扫描器，对格子和区域进行扫描来找寻场景对象
**/
#ifndef SCENE_SCANER_H_
#define SCENE_SCANER_H_

#include "scene/config.h"
#include "scene/define.h"

namespace scene {

class PF_PLUGIN_API Scaner {

 public:
   Scaner();
   virtual ~Scaner();

 public:
   virtual bool init(scaner_init_t *initdata);
   virtual bool on_beforescan();
   virtual bool on_needscan(gridid_t gridid);
   virtual uint32_t on_findobject(structure::Object *object);
   virtual void on_afterscan();

 public:
   Scene *getscene();

 protected:
   Scene *scene_;
   gridid_t gridid_;
   int32_t gridradius_;
   bool scanhuman_;

 private:
   friend class Scene;

};

}; //namespace scene

#endif //SCENE_SCANER_H_
