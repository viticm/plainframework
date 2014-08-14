#include "connection/counter/center.h"

connection::counter::Center *g_connection_counter_center = NULL;

template <>
connection::counter::Center 
  *pf_base::Singleton<connection::counter::Center>::singleton_ = NULL;

namespace connection {

namespace counter {

Center::Center() {
  __ENTER_FUNCTION
    center_playercount_ = 0;
    playercount_max_ = 1024;
  __LEAVE_FUNCTION
}

Center::~Center() {
  //do nothing
}

Center *Center::getsingleton_pointer() {
  return singleton_;
}

Center &Center::getsingleton() {
  Assert(singleton_);
  return *singleton_;
}

uint16_t Center::get_require_playercount() const {
  __ENTER_FUNCTION
    int32_t count = playercount_max_ - center_playercount_;
    uint16_t result = count > 0 ? count : 0;
    return result;
  __LEAVE_FUNCTION
    return 0;
}

void Center::set_center_playercount(uint16_t count) {
  center_playercount_ = count;
}

uint16_t Center::get_center_playercount() const {
  return center_playercount_;
}

void Center::set_playercount_max(uint16_t count) {
  playercount_max_ = count;
}

uint16_t Center::get_playercount_max() const {
  return playercount_max_;
}

} //namespace counter

} //namespace connection
