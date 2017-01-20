#include "pf/base/util.h"
#include "pf/base/time_manager.h"
#include "pf/base/log.h"
#include "pf/sys/thread.h"
#include "scene/scene.h"
#include "scene/thread.h"

namespace scene {

Thread::Thread() {
  __ENTER_FUNCTION
    for (int32_t i = 0; i < SCENE_THREAD_PER_MAX; ++i) {
      scenes_[i] = NULL;
      scenesbak_[i] = NULL;
    }
    luainterface_.init();
    scenecount_ = 0;
    isactive_ = true;
    ticktimes_ = 0;
    ticksteps_ = 0;
    isinit_ = false;
    scenecount_bak_ = 0;
    reload_scriptid_ = ID_INVALID;
  __LEAVE_FUNCTION
}

Thread::~Thread() {
  __ENTER_FUNCTION
    luainterface_.release();
  __LEAVE_FUNCTION
}

void Thread::run() {
  __ENTER_FUNCTION
    uint64_t threadid = get_id();
    for (int32_t i = 0; i < scenecount_; ++i) {
      scenes_[i]->threadid_ = threadid + scenes_[i]->getid();
    }
    THREAD_INDENT_POINTER->add(threadid);
    if (0 == scenecount_) {
      SLOW_LOG(SCENE_MODULENAME, 
               "[scene] (Thread::run) no scene add!"
               " thread id: %"PRIu64"",
               threadid);
      return;
    }
    scenecount_bak_ = scenecount_;
    for (int32_t i = 0; i < scenecount_; ++i) {
      SLOW_LOG(SCENE_MODULENAME,
               "[scene] (Thread::run) logging..."
               " thread id: %d, scene id: %d,"
               " scene thread id: %d, scene map file name: %s",
               threadid,
               scenes_[i]->getid(),
               scenes_[i]->threadid_,
               scenes_[i]->map_filename_);
    }
    uint32_t ticktime = TIME_MANAGER_POINTER->get_tickcount();
    //int32_t old_scenecount = scenecount_;
    for (int32_t i = 0; i < scenecount_; ++i) {
      scenesbak_[i] = scenes_[i];
    }
    while (isactive()) {
      ++tickcount_;
      steps_ = 0;
      uint32_t current_ticktime = TIME_MANAGER_POINTER->get_tickcount();
      uint32_t delta = current_ticktime - ticktime;
      ticktime = current_ticktime;
      if (delta > 90) {
        pf_base::util::sleep(10);
      } else {
        pf_base::util::sleep(100 - delta);
      }
      steps_ = 1;
      if (scenecount_ != scenecount_bak_) Assert(false);
      steps_ = 2;
      if (0 == scenecount_ || scenecount_ > SCENE_THREAD_PER_MAX) Assert(false);
      steps_ = 3;
      for (uint16_t i = 0; i < scenecount_; ++i) {
        Assert(scenecount_ == scenecount_bak_);
        Assert(scenes_[i] == scenesbak_[i]);
        ++(scenes_[i]->tick_);
        THREAD_INDENT_POINTER->setindent(
            pf_sys::get_origine_thread_id(), 
            static_cast<int32_t>(scenes_[i]->getid()));
        bool tickresult = scenes_[i]->tick();
        ++(scenes_[i]->tick_);
        Assert(tickresult);
      }
      steps_ = 4;
      ++tickcount_;
      if (!isinit() && tickcount_ > 10) isinit_ = true;
      int32_t scriptid = get_reload_scriptid();
      if (scriptid != ID_INVALID) luainterface_.reloadscript(scriptid);
    }
    quit();
  __LEAVE_FUNCTION
}

void Thread::stop() {
  isactive_ = false;
}

bool Thread::isactive() const {
  return isactive_;
}
   
void Thread::quit() {
  __ENTER_FUNCTION
    SLOW_LOG(SCENE_MODULENAME,
             "[scene] (Thread::quit) id: %d",
             get_id());
  __LEAVE_FUNCTION
}
   
bool Thread::addscene(Scene *scene) {
  __ENTER_FUNCTION
    if (scenecount_ > SCENE_THREAD_PER_MAX) return false;
    scenes_[scenecount_] = scene;
    scene->setthread(this);
    ++scenecount_;
    return true;
  __LEAVE_FUNCTION
    return false;
}

int32_t Thread::get_scenecount() const {
  return scenecount_;
}
   
bool Thread::isinit() const {
  return isinit_;
}
   
pf_script::lua::Interface *Thread::get_luainterface() {
  return &luainterface_;
}
   
int32_t Thread::get_reload_scriptid() {
  __ENTER_FUNCTION
    pf_sys::lock_guard<pf_sys::ThreadLock> autolock(lock_);
    int32_t result = reload_scriptid_;
    reload_scriptid_ = ID_INVALID;
    return result;
  __LEAVE_FUNCTION
    return ID_INVALID;
}
   
void Thread::set_reload_scriptid(int32_t scriptid) {
  __ENTER_FUNCTION
    pf_sys::lock_guard<pf_sys::ThreadLock> autolock(lock_);
    reload_scriptid_ = scriptid;
  __LEAVE_FUNCTION
}

} //namespace scene
