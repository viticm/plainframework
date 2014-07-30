#include "server/common/game/db/struct.h"

namespace pap_server_common_game {

namespace db {

namespace share_memory {

head_t::head_t() {
  __ENTER_FUNCTION
    clean_up();
  __LEAVE_FUNCTION
}

head_t::~head_t() {
  //do nothing
}

void head_t::clean_up() {
  __ENTER_FUNCTION
    pool_id = 0;
    id = -1;
    player_id = -1;
    use_status = pap_server_common_sys::share_memory::kUseFree;
    flag = pap_server_common_sys::share_memory::kFlagFree;
    save_time = 0;
  __LEAVE_FUNCTION
}

void global_data_t::init() {
  __ENTER_FUNCTION
    global_data = 0;
  __LEAVE_FUNCTION
}

void global_data_t::lock(char type) {
  __ENTER_FUNCTION
    pap_server_common_sys::share_memory::lock(head.flag, type);
  __LEAVE_FUNCTION
}

void global_data_t::unlock(char type) {
  __ENTER_FUNCTION
    pap_server_common_sys::share_memory::unlock(head.flag, type);
  __LEAVE_FUNCTION
}

void global_data_t::set_pool_id(uint32_t id) {
  __ENTER_FUNCTION
    head.pool_id = id;
  __LEAVE_FUNCTION
}

uint32_t global_data_t::get_pool_id() {
  __ENTER_FUNCTION
    return head.pool_id;
  __LEAVE_FUNCTION
    return 0;
}

bool global_data_t::set_use_status(int32_t status, char type) {
  __ENTER_FUNCTION
    lock(type);
    head.use_status = status;
    unlock(type);
    return true;
  __LEAVE_FUNCTION
    return false;
}

int32_t global_data_t::get_use_status(char type) {
  __ENTER_FUNCTION
    int32_t result = -1;
    lock(type);
    result = head.use_status;
    unlock(type);
    return result;
  __LEAVE_FUNCTION
    return -1;
}

uint32_t global_data_t::get_save_time(char type) {
  __ENTER_FUNCTION
    uint32_t save_time = 0;
    lock(type);
    save_time = head.save_time;
    unlock(type);
    return save_time;
  __LEAVE_FUNCTION
    return 0;
}

void global_data_t::set_save_time(uint32_t time, char type) {
  __ENTER_FUNCTION
    lock(type);
    head.save_time = time;
    unlock(type);
  __LEAVE_FUNCTION
}

uint32_t global_data_t::get_data(char type) {
  __ENTER_FUNCTION
    uint32_t data;
    lock(type);
    data = global_data;
    unlock(type);
    return data;
  __LEAVE_FUNCTION
    return 0;
}

void global_data_t::set_data(char type, uint32_t data) {
  __ENTER_FUNCTION
    lock(type);
    global_data = data;
    unlock(type);
  __LEAVE_FUNCTION
}

} //namespace share_memory

} //namespace db

} //namespace pap_server_common_game
