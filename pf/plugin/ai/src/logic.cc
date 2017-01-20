#include "pf/base/log.h"
#include "ai/manager.h"
#include "ai/action.h"
#include "ai/state.h"
#include "ai/logic.h"

namespace ai {

Logic::Logic() {
  __ENTER_FUNCTION
    type_ = -1;
    scriptid_ = -1;
    initstate_ = -1;
  __LEAVE_FUNCTION
}

Logic::~Logic() {
  //do nothing
}

bool Logic::init(int8_t type, int32_t scriptid) {
  __ENTER_FUNCTION
    actiontable_.clear();
    user_actiontable_.clear();
    statetable_.clear();
    type_ = type;
    scriptid_ = scriptid;
    if (Manager::self_ && 
        Manager::self_->get_luainterface() && 
        scriptid_ != ID_INVALID) {
      Manager::self_->get_luainterface()->run_scriptfunction(
          scriptid_, "setup", POINTER_TOINT64(this));
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

Action *Logic::getaction(int16_t id) {
  __ENTER_FUNCTION
    Action *action = NULL;
    actiontable_t::iterator _iteractor = actiontable_.find(id);
    if (_iteractor != actiontable_.end()) action = &_iteractor->second;
    return action;
  __LEAVE_FUNCTION
    return NULL;
}
   
State *Logic::getstate(int16_t id) {
  __ENTER_FUNCTION
    State *state = NULL;
    statetable_t::iterator _iteractor = statetable_.find(id);
    if (_iteractor != statetable_.end()) state = &_iteractor->second;
    return state;
  __LEAVE_FUNCTION
    return NULL;
}
   
int16_t Logic::get_initstate() const {
  return initstate_;
}
   
int32_t Logic::get_scriptid() const {
  return scriptid_;
}
   
action_t Logic::callaction(structure::Object *object, action_t actiondata) {
    action_t handle;
  __ENTER_FUNCTION
    if (NULL == Manager::self_) return handle;
    action_t nexthandle;
    Action *action = actiondata.action;
    int32_t actionkey = 0;
    if (!action) action = getaction(actiondata.id);
    if (NULL == action) return handle;
    actionkey = action->key_;
    if (actionkey < AI_USERACTION_START) {
      int32_t branchindex = 0;
      function_action actionfunction = 
        Manager::self_->get_actionfunction(actionkey);
      Assert(actionfunction);
      branchindex = (*actionfunction)(object, action);
      if (branchindex >= 0 && branchindex < AI_ACTION_BRANCH_COUNT) {
        int16_t next_actionid = action->branchs_[branchindex].id;
        Action *next_aciton = action->branchs_[branchindex].action;
        if (!next_aciton) {
          next_aciton = getaction(next_actionid);
          action->branchs_[branchindex].action = next_aciton;
        }
        nexthandle.id = next_actionid;
        nexthandle.action = next_aciton;
      }
      if (-1 == branchindex) {
        nexthandle.id = ID_INVALID;
        nexthandle.action = NULL;
      }
    } else { //用户的数据走脚本
      if (!Manager::self_->get_luainterface() || ID_INVALID == scriptid_) 
        return handle;
      int32_t branchindex = 0;
      user_actiontable_t::iterator _iteractor;
      _iteractor = user_actiontable_.find(actionkey);
      if (_iteractor == user_actiontable_.end()) return handle;
      branchindex = Manager::self_->get_luainterface()->run_scriptfunction(
          scriptid_, 
          _iteractor->second.c_str(), 
          POINTER_TOINT64(object), 
          POINTER_TOINT64(action));
      if (branchindex >= 1 && branchindex <= AI_ACTION_BRANCH_COUNT) {
        int16_t next_actionid = action->branchs_[branchindex - 1].id;
        Action *next_aciton = action->branchs_[branchindex - 1].action;
        if (!next_aciton) {
          next_aciton = getaction(next_actionid);
          action->branchs_[branchindex - 1].action = next_aciton;
        }
        nexthandle.id = next_actionid;
        nexthandle.action = next_aciton;
      }
    }
    handle = nexthandle;
    return handle;
  __LEAVE_FUNCTION
    return handle;
}
   
void Logic::newaction(int32_t actionid, int32_t actionkey) {
  __ENTER_FUNCTION
    if (ID_INVALID == actionid || ID_INVALID == actionkey) return;
    if (actionkey < AI_USERACTION_START) {
      if (NULL == Manager::self_) return;
      function_action actionfunction = 
        Manager::self_->get_actionfunction(actionkey);
      if (NULL == actionfunction) return;
    } else {
      user_actiontable_t::iterator _iteractor;
      _iteractor = user_actiontable_.find(actionkey);
      if (_iteractor == user_actiontable_.end()) return;
    }
    Action *action = NULL;
    std::pair<actiontable_t::iterator, bool> pair;
    pair = actiontable_.insert(std::make_pair(actionid, Action()));
    if (!pair.second) {
      SLOW_ERRORLOG(AI_MODULENAME, 
                    "[ai] (Logic::newaction) action table insert error,"
                    " action id: %d, key: %d, ai type: %d",
                    actionid,
                    actionkey,
                    type_);
      return;
    }
    action = &(pair.first->second);
    action->key_ = actionkey;
  __LEAVE_FUNCTION
}
   
void Logic::newstate(int16_t state) {
  __ENTER_FUNCTION
    std::pair<statetable_t::iterator, bool> pair;
    if (state < 0) return;
    pair = statetable_.insert(std::make_pair(state, State(this)));
    if (!pair.second) {
      SLOW_ERRORLOG(AI_MODULENAME, 
                    "[ai] (Logic::newstate) action table insert error,"
                    " state: %d, ai type: %d",
                    state,
                    type_);
      return;
    }
  __LEAVE_FUNCTION
}

void Logic::set_initstate(int16_t state) {
  __ENTER_FUNCTION
    State *_state = getstate(state);
    Assert(_state);
    initstate_ = state;
  __LEAVE_FUNCTION
}
   
void Logic::register_useraction(int32_t key, const char *functionname) {
  __ENTER_FUNCTION
    user_actiontable_[key] = functionname;
  __LEAVE_FUNCTION
}

} //namespace ai
