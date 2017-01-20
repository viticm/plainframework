#include "pf/file/database.h"
#include "ai/define.h"
#include "ai/logic.h"
#include "ai/manager.h"

namespace ai {

Manager *Manager::self_ = NULL;

Manager::Manager() {
  __ENTER_FUNCTION
    table_.clear();
  __LEAVE_FUNCTION
}

Manager::~Manager() {
  __ENTER_FUNCTION
    cleanup();
  __LEAVE_FUNCTION
}

bool Manager::init(const char *filename) {
  __ENTER_FUNCTION
    bool result = false;
    result = loadconfig(filename);
    return result;
  __LEAVE_FUNCTION
    return false;
}

void Manager::cleanup() {
  __ENTER_FUNCTION
    table_t::iterator _iteractor;
    for (_iteractor = table_.begin(); 
         _iteractor != table_.end(); 
         ++_iteractor) {
      SAFE_DELETE(_iteractor->second.logic);
    }
    table_.clear();
    luainterface_ = NULL;
  __LEAVE_FUNCTION
}

void Manager::set_luainterface(pf_script::lua::Interface *luainterface) {
  luainterface_ = luainterface;
}

bool Manager::loadconfig(const char *filename) {
  __ENTER_FUNCTION
    bool result = false;
    pf_file::Database tabfile(0);
    result = tabfile.open_from_txt(filename);
    info_t info;
    if (!result) return result;
    for (int16_t line = 0; line < tabfile.get_record_number(); ++line) {
      int16_t type = 0;
      int32_t scriptid = ID_INVALID;
      type = static_cast<int16_t>(
          tabfile.get_fielddata(line, "AIType")->int_value);
      Assert(type >= 0);
      scriptid = tabfile.get_fielddata(line, "ScriptId")->int_value;
      info.scriptid = scriptid;
      info.logic = NULL;
      table_[type] = info;
    }
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Manager::reloadlogic(const char *filename, int16_t type) {
  __ENTER_FUNCTION
    if (NULL == luainterface_) return false;
    bool result = false;
    pf_file::Database tabfile(0);
    result = tabfile.open_from_txt(filename);
    if (!result) return result;

    int32_t scriptid = ID_INVALID;
    for (int16_t line = 0; line < tabfile.get_record_number(); ++line) {
      int16_t _type = 0;
      type = static_cast<int16_t>(
          tabfile.get_fielddata(line, "AIType")->int_value);
      Assert(type >= 0);
      if (_type != type) continue;
      scriptid = tabfile.get_fielddata(line, "ScriptId")->int_value;
    }
    result = luainterface_->reloadscript(scriptid);
    table_t::iterator _iteractor = table_.find(type);
    if (_iteractor == table_.end() || NULL == _iteractor->second.logic) {
      table_[type].scriptid = scriptid;
      table_[type].logic = create(type, scriptid);
    } else {
      _iteractor->second.scriptid = scriptid;
      result = _iteractor->second.logic->init(type, scriptid);
      Assert(result);
    }
    return result;
  __LEAVE_FUNCTION
    return false;
}

Logic *Manager::get(int16_t type) {
  __ENTER_FUNCTION
    Logic *logic = NULL;
    info_t *info = NULL;
    table_t::iterator _iteractor = table_.find(type);
    Assert(_iteractor != table_.end());
    info = &_iteractor->second;
    if (NULL == info->logic) info->logic = create(type, info->scriptid);
    logic = info->logic;
    return logic;
  __LEAVE_FUNCTION
    return NULL;
}

Logic *Manager::create(int16_t type, int32_t scriptid) {
  __ENTER_FUNCTION
    Logic *logic = NULL;
    logic = new Logic;
    Assert(logic);
    if (!logic->init(type, scriptid)) {
      SAFE_DELETE(logic);
    }
    return logic;
  __LEAVE_FUNCTION
    return NULL;
}

pf_script::lua::Interface *Manager::get_luainterface() {
  return luainterface_;
}

} //namespace ai
