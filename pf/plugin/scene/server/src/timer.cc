#include "pf/base/time_manager.h"
#include "pf/base/log.h"
#include "pf/base/string.h"
#include "scene/scene.h"
#include "scene/timer.h"

namespace scene {

Timer::Timer() {
  __ENTER_FUNCTION
    timers_ = NULL;
    system_timers_ = NULL;
    scene_ = NULL;
    timercount_max_ = 0;
    dumptimers_ = false;
  __LEAVE_FUNCTION
}

Timer::~Timer() {
  __ENTER_FUNCTION
    SAFE_DELETE_ARRAY(system_timers_);
    SAFE_DELETE_ARRAY(timers_);
  __LEAVE_FUNCTION
}

void Timer::cleanup() {
  __ENTER_FUNCTION
    int32_t i = 0;
    for (i = 0; i < timercount_max_; ++i) normalinit(i);
    for (i = 0; i < SCENE_TIMER_DOING_NUMBER; ++i) systeminit(i);
    dumptimers_ = false;
    uint32_t nowtick = TIME_MANAGER_POINTER->get_tickcount();
    refeshtimer_.start(1000, nowtick);
  __LEAVE_FUNCTION
}

bool Timer::isinit() const {
  bool result = scene_ != NULL;
  return result;
}

void Timer::init(int32_t countmax, Scene *scene) {
  __ENTER_FUNCTION
    timercount_max_ = countmax;
    SAFE_DELETE_ARRAY(system_timers_);
    SAFE_DELETE_ARRAY(timers_);
    timers_ = new timer_t[timercount_max_];
    system_timers_ = new timer_t[SCENE_TIMER_DOING_NUMBER];
    int32_t i = 0;
    for (i = 0; i < timercount_max_; ++i) normalinit(i);
    for (i = 0; i < SCENE_TIMER_DOING_NUMBER; ++i) systeminit(i);
    uint32_t nowtick = TIME_MANAGER_POINTER->get_tickcount();
    refeshtimer_.start(1000, nowtick);
    scene_ = scene;
  __LEAVE_FUNCTION
}

void Timer::normalinit(int32_t index) {
  __ENTER_FUNCTION
    Assert(index >= 0 && index < timercount_max_);
  if (index >= 0 && index < timercount_max_) {
      memset(timers_[index].script_functionname,
             0, 
             sizeof(timers_[index].script_functionname));
      timers_[index].scriptid = ID_INVALID;
      timers_[index].id = ID_INVALID;
      timers_[index].timer.cleanup();
    }
  __LEAVE_FUNCTION
}

int32_t Timer::get() {
  __ENTER_FUNCTION
    int32_t result = -1;
    for (int32_t i = 0; i < timercount_max_; ++i) {
      if (false == timers_[i].timer.isstart()) {
        result = i;
        break;
      }
    }
    if (-1 == result && false == dumptimers_) {
      dumptimers_ = true;
      FAST_ERRORLOG(kErrorLogFile,
                    "[scene] (Timer::get) dump timers"
                    " scene(%d) timer overflow...",
                    scene_ ? scene_->getid() : -1);
      for (uint16_t i = 0; i < timercount_max_; ++i) {
        if (strlen(timers_[i].script_functionname) > 0) {
          FAST_ERRORLOG(kErrorLogFile,
                        "[scene] (Timer::get) dump timers"
                        " timer index: %d, function name: %s",
                        i,
                        timers_[i].script_functionname);
        }
      }
    }
    return result;
  __LEAVE_FUNCTION
    return -1;
}

void Timer::free(int32_t index) {
  __ENTER_FUNCTION
    Assert(index >= 0 && index < timercount_max_);
    normalinit(index);
  __LEAVE_FUNCTION
}

bool Timer::check(int32_t index) {
  __ENTER_FUNCTION
    Assert(index >= 0 && index < timercount_max_);
    bool result = timers_[index].timer.isstart();
    return result;
  __LEAVE_FUNCTION
    return false;
}

void Timer::normalstart(int32_t index, 
                        int32_t id, 
                        int32_t scriptid, 
                        const char *functionname, 
                        int32_t ticktime) {
  __ENTER_FUNCTION
    Assert(index >= 0 && index < timercount_max_);
    if (NULL == functionname) return;
    size_t functionname_length = strlen(functionname);
    if (functionname_length > kScriptFunctionNameLength) return;
    if (check(index)) return;
    normalinit(index);
    pf_base::string::safecopy(timers_[index].script_functionname,
                              functionname,
                              sizeof(timers_[index].script_functionname));
    timers_[index].id = id;
    timers_[index].scriptid = scriptid;
    uint32_t nowtick = TIME_MANAGER_POINTER->get_tickcount();
    timers_[index].timer.start(ticktime, nowtick);
  __LEAVE_FUNCTION
}

void Timer::systeminit(int32_t index) {
  __ENTER_FUNCTION
    Assert(index >= 0 && index < SCENE_TIMER_DOING_NUMBER);
  if (index >= 0 && index < SCENE_TIMER_DOING_NUMBER) {
      memset(system_timers_[index].script_functionname,
             0, 
             sizeof(system_timers_[index].script_functionname));
      system_timers_[index].scriptid = ID_INVALID;
      system_timers_[index].id = ID_INVALID;
      system_timers_[index].timer.cleanup();
    }
  __LEAVE_FUNCTION
}

void Timer::systemstart(int32_t index, 
                        int32_t scriptid, 
                        const char *functionname, 
                        int32_t ticktime) {
  __ENTER_FUNCTION
    Assert(index >= 0 && index < SCENE_TIMER_DOING_NUMBER);
    if (NULL == functionname) return;
    size_t functionname_length = strlen(functionname);
    if (functionname_length > kScriptFunctionNameLength) return;
    if (system_timers_[index].timer.isstart()) return;
    systeminit(index);
    pf_base::string::safecopy(
        system_timers_[index].script_functionname,
        functionname,
        sizeof(system_timers_[index].script_functionname));
    system_timers_[index].scriptid = scriptid;
    uint32_t nowtick = TIME_MANAGER_POINTER->get_tickcount();
    system_timers_[index].timer.start(ticktime, nowtick);
  __LEAVE_FUNCTION
}

int32_t Timer::system_remaintime(int32_t index) {
  __ENTER_FUNCTION
    Assert(index >= 0 && index < SCENE_TIMER_DOING_NUMBER);
    uint32_t termtime = system_timers_[index].timer.get_termtime();
    uint32_t last_ticktime = system_timers_[index].timer.get_last_ticktime();
    uint32_t now_ticktime = TIME_MANAGER_POINTER->get_tickcount();
    int32_t result = termtime + last_ticktime - now_ticktime;
    return result;
  __LEAVE_FUNCTION
    return 0;
}

void Timer::onevent(uint32_t time) {
  __ENTER_FUNCTION
    for (int32_t i = 0; i < timercount_max_; ++i) {
      if (timers_[i].timer.isstart()) {
        if (timers_[i].timer.counting(time)) {
          getscene()->get_luainterface()->run_scriptfunction(
              timers_[i].scriptid,
              timers_[i].script_functionname,
              static_cast<int64_t>(getscene()->getid()),
              static_cast<int64_t>(timers_[i].id),
              static_cast<int64_t>(i),
              0);
        }
      }
    } //for
  __LEAVE_FUNCTION
}

Scene *Timer::getscene() {
  return scene_;
}

} //namespace scene
