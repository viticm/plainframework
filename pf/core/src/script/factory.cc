#include "pf/script/interface.h"
#include "pf/script/lua/system.h"
#include "pf/basic/logger.h"
#include "pf/script/factory.h"

using namespace pf_script;

eid_t Factory::newenv(const config_t &config) {
  eid_t eid = neweid();
  if (SCRIPT_EID_INVALID == eid) return eid;
  Interface *env = nullptr;
  switch (config.type) {
    case kTypeLua:
#ifdef PF_OPEN_LUA
      env = new lua::System();
      env->set_rootpath(config.rootpath);
      env->set_workpath(config.workpath);
#else
      SLOW_ERRORLOG(SCRIPT_MODULENAME, "create the lua env error: not open");
#endif
      break;
    default:
      break;
  }
  std::unique_ptr< Interface > pointer(env);
  envs_[eid] = std::move(pointer);
  return eid;
}

eid_t Factory::newenv(Interface *env) {
  eid_t eid = neweid();
  if (SCRIPT_EID_INVALID == eid) return eid;
  std::unique_ptr< Interface > pointer(env);
  envs_[eid] = std::move(pointer);
  return eid;
}

eid_t Factory::neweid() {
  eid_t eid = SCRIPT_EID_INVALID;
  eid = SCRIPT_EID_INVALID == last_del_eid_ ?
        static_cast<eid_t>(envs_.size() + 1) : last_del_eid_;
  return eid;
}
