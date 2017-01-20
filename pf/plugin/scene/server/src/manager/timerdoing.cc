#include "pf/base/log.h"
#include "scene/scene.h"
#include "scene/manager/timerdoing.h"

namespace scene {

namespace manager {

TimerDoing::TimerDoing() {
  cleanup();
}

TimerDoing::~TimerDoing() {
  //do nothing
}

bool TimerDoing::init() {
  return true;
}

void TimerDoing::cleanup() {
  __ENTER_FUNCTION
    for (uint16_t i = 0; i < SCENE_MANAGER_TIMERDOING_MAX; ++i)
      is_started_[i] = false;
    scene_ = NULL;
  __LEAVE_FUNCTION
}

void TimerDoing::start(int32_t scriptid,
                       sceneid_t sceneid,
                       int32_t index,
                       int32_t param[SCENE_MANAGER_TIMERDOING_PARAMNUM]) {
  __ENTER_FUNCTION
    if (index > SCENE_MANAGER_TIMERDOING_MAX) return;
    if (is_started_[index] == true) {
#ifdef _DEBUG
      FAST_LOG(kDebugLogFile,
               "[scene.manager] (TimerDoing::start) timer is started,"
               " index: %d",
               index);
#endif
      return;
    }
    is_started_[index] = true;
    Assert(sceneid == scene_->getid());
    if (scriptid > 0 && scene_->getconfig_boolvalue(kConfigScriptIsActive)) {
      scene_->get_luainterface()->run_scriptfunction(
          scriptid,
          "on_timerdoing_start",
          sceneid,
          index,
          param[0],
          param[1],
          param[2],
          param[3],
          param[4]);
    }
  __LEAVE_FUNCTION
}

void TimerDoing::setscene(scene::Scene *scene) {
  scene_ = scene;
}

scene::Scene *TimerDoing::getscene() {
  return scene_;
}

} //namespace manager

} //namespace scene
