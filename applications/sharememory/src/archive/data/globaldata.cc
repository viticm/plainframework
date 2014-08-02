#include "archive/data/globaldata.h"

using namespace archive::data;
GlobalData::GlobalData(pf_db::Manager *db_manager) {
  db_manager_ = db_manager;
  db_type_ = common::db::kTypeCharacter;
}

GlobalData::~GlobalData() {
  //do nothing
}

bool GlobalData::load() {
  __ENTER_FUNCTION
    
  __LEAVE_FUNCTION
    return false;
}

bool GlobalData::add() {
  __ENTER_FUNCTION

  __LEAVE_FUNCTION
    return false;
}

bool GlobalData::erase() {
  __ENTER_FUNCTION

  __LEAVE_FUNCTION
    return false;
}

bool GlobalData::save(void *source) {
  __ENTER_FUNCTION

  __LEAVE_FUNCTION
    return false;
}

bool GlobalData::fetch(void *source) {
  __ENTER_FUNCTION

  __LEAVE_FUNCTION
    return false;
}
