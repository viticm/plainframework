#include "pf/sys/memory/share.h"
#include "common/sharememory/define.h"

namespace common {

namespace sharememory {

head_struct::head_struct() {
  __ENTER_FUNCTION
    cleanup();
  __LEAVE_FUNCTION
}

head_struct::~head_struct() {
  //do nothing
}

void head_struct::cleanup() {
  __ENTER_FUNCTION
    using namespace pf_sys::memory::share;
    poolid = 0;
    id = -1;
    playerid = -1;
    usestatus = kUseStatusFree;
    flag = kFlagFree;
    savetime = 0;
  __LEAVE_FUNCTION
}

void globaldata_struct::init() {
  __ENTER_FUNCTION
    data = 0;
  __LEAVE_FUNCTION
}

void globaldata_struct::lock(char type) {
  __ENTER_FUNCTION
    pf_sys::memory::share::lock(head.flag, type);
  __LEAVE_FUNCTION
}

void globaldata_struct::unlock(char type) {
  __ENTER_FUNCTION
    pf_sys::memory::share::unlock(head.flag, type);
  __LEAVE_FUNCTION
}

void globaldata_struct::set_poolid(uint32_t id) {
  __ENTER_FUNCTION
    head.poolid = id;
  __LEAVE_FUNCTION
}

uint32_t globaldata_struct::get_poolid() {
  __ENTER_FUNCTION
    return head.poolid;
  __LEAVE_FUNCTION
    return 0;
}

bool globaldata_struct::set_usestatus(int32_t status, char type) {
  __ENTER_FUNCTION
    lock(type);
    head.usestatus = status;
    unlock(type);
    return true;
  __LEAVE_FUNCTION
    return false;
}

int32_t globaldata_struct::get_usestatus(char type) {
  __ENTER_FUNCTION
    int32_t result = -1;
    lock(type);
    result = head.usestatus;
    unlock(type);
    return result;
  __LEAVE_FUNCTION
    return -1;
}

uint32_t globaldata_struct::get_savetime(char type) {
  __ENTER_FUNCTION
    uint32_t savetime = 0;
    lock(type);
    savetime = head.savetime;
    unlock(type);
    return savetime;
  __LEAVE_FUNCTION
    return 0;
}

void globaldata_struct::set_savetime(uint32_t time, char type) {
  __ENTER_FUNCTION
    lock(type);
    head.savetime = time;
    unlock(type);
  __LEAVE_FUNCTION
}

uint32_t globaldata_struct::getdata(char type) {
  __ENTER_FUNCTION
    uint32_t _data;
    lock(type);
    _data = data;
    unlock(type);
    return _data;
  __LEAVE_FUNCTION
    return 0;
}

void globaldata_struct::setdata(char type, uint32_t data) {
  __ENTER_FUNCTION
    lock(type);
    data = data;
    unlock(type);
  __LEAVE_FUNCTION
}

} //namespace sharememory

} //namespace common
