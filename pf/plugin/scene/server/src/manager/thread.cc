#include "pf/base/log.h"
#include "pf/base/util.h"
#include "scene/thread.h"
#include "scene/manager/thread.h"

namespace scene {

namespace manager {

Thread::Thread() {
  __ENTER_FUNCTION
    count_ = 0;
    pool_ = NULL;
    isinit_ = false;
  __LEAVE_FUNCTION
}

Thread::~Thread() {
  __ENTER_FUNCTION
    SAFE_DELETE(pool_);
  __LEAVE_FUNCTION
}

bool Thread::init(settings_t &settings, int32_t serverid, uint16_t countmax) {
  __ENTER_FUNCTION
    if (isinit()) return true;
    pool_ = new pf_sys::ThreadPool;
    uint16_t scenecount = settings.count;
    Assert(scenecount <= SCENE_MAX);
    uint32_t optionindex = 0;
    int32_t current_threadindex = 0;
    for (uint16_t i = 0; i < scenecount; ++i) {
      sceneid_t sceneid = settings.scenes[i].id;
      Assert(sceneid < SCENE_MAX);
      if (settings.scenes[i].serverid != serverid) continue;
      if (!settings.scenes[i].isactive) continue;
      settings.scenes[i].threadindex = current_threadindex++;
      if (current_threadindex >= countmax) current_threadindex = 0;
      int32_t threadindex = settings.scenes[i].threadindex;
      scene::Thread *thread = 
        dynamic_cast<scene::Thread *>(pool_->get_byindex(threadindex));
      if (NULL == thread) {
        thread = new scene::Thread;
        Assert(thread);
        bool result = pool_->add(thread, threadindex);
        Assert(result);
        ++count_;
      }
      SLOW_LOG(SCENE_MODULENAME,
               "[scene.manager] (Thread::init)"
               " index: %03d, scene id: %04d, thread index: %03d",
               optionindex++,
               sceneid,
               threadindex);
    }
    isinit_ = true;
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Thread::start() {
  __ENTER_FUNCTION
    bool result = pool_->start();
    result = false;
    while (!result) {
      pf_base::util::sleep(500);
      result = pool_->is_allstarted();
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Thread::stop() {
  __ENTER_FUNCTION
    if (pool_) pool_->stop();
    return true;
  __LEAVE_FUNCTION
    return false;
}

pf_sys::ThreadPool *Thread::getpool() {
  return pool_;
}

uint16_t Thread::getcount() const {
  return count_;
}

bool Thread::isinit() const {
  return isinit_;
}

} //namespace manager

} //namespace scene
