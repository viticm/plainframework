#include "archive/data/interface.h"

namespace archive {

namespace data {

Interface::Interface() {
  db_manager_ = NULL;
  db_type_ = common::db::kTypeCharacter; //默认为角色数据库
}

bool Interface::load() {
  __ENTER_FUNCTION
    if (!db_manager_ || !db_manager_->isready()) return false;
    db_optiontype_ = kDBOptionTypeLoad;
    bool result = db_manager_->query();
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Interface::add() {
  __ENTER_FUNCTION
    if (!db_manager_ || !db_manager_->isready()) return false;
    db_optiontype_ = kDBOptionTypeAdd;
    bool result = db_manager_->query();
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Interface::erase() {
  __ENTER_FUNCTION
    if (!db_manager_ || !db_manager_->isready()) return false;
    db_optiontype_ = kDBOptionTypeErase;
    bool result = db_manager_->query();
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Interface::save(void *source) {
  __ENTER_FUNCTION
    USE_PARAM(source);
    if (!db_manager_ || !db_manager_->isready()) return false;
    db_optiontype_ = kDBOptionTypeSave;
    bool result = db_manager_->query();
    return result;
  __LEAVE_FUNCTION
    return false;
}

} //namespace data

} //namespace archive
