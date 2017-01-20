/**
#include "pf/base/log.h"
#include "logic/systeminterface.h"
#include "logic/manager.h"

namespace logic {

Manager::Manager() {
  //do nothing
}

Manager::~Manager() {
  //do nothing
}

bool Manager::init(uint16_t systemcount) {
  __ENTER_FUNCTION
    systemcount = 0 == systemcount ? LOGIC_SYSTEMCOUNT_MAX : systemcount;
    hashmap_.init(systemcount);
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Manager::registersystem(SystemInterface *system) {
  __ENTER_FUNCTION
    if (hashmap_.isfind(system->getid())) {
      SLOW_WARNINGLOG(LOGIC_MODULENAME, 
                      "[logic] (Manager::registersystem) repeat add"
                      " system id: %d",
                      system->getid());
      return false;
    }
    bool result = hashmap_.add(system->getid(), system);
    return result;
  __LEAVE_FUNCTION
    return false;
}

SystemInterface *Manager::getsystem(uint16_t id) {
  __ENTER_FUNCTION
    SystemInterface *result = NULL;
    if (hashmap_.isfind(id))
      result = hashmap_.get(id);
    return result;
  __LEAVE_FUNCTION
    return NULL;
}

} //namespace logic
**/
