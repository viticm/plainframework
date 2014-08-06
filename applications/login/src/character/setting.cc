#include "character/setting.h"
#include "pf/base/string.h"

namespace character {

new_struct::new_struct() {
  memset(name, 0, sizeof(name));
}

new_struct::~new_struct() {
  //do nothing
}

void new_struct::setguid(int64_t _guid) {
  guid = _guid;
}

int64_t new_struct::getguid() {
  return guid;
}

void new_struct::setname(const char *_name) {
  pf_base::string::safe_copy(name, _name, sizeof(name));
}

const char *new_struct::getname() {
  return name;
}

void new_struct::setsex(uint8_t _sex) {
  sex = _sex;
}

uint8_t new_struct::getsex() {
  return sex;
}

void new_struct::setprofession(uint8_t _profession) {
  profession = _profession;
}

uint8_t new_struct::getprofession() {
  return profession;
}

} //namespace character
