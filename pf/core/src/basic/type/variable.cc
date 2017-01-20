#include "pf/basic/type/variable.h"

namespace pf_basic {

namespace type {

variable_struct::variable_struct() {
  type = kVariableTypeInvalid;
}

variable_struct::variable_struct(const variable_t &object) {
  data = object.data;
  type = object.type;
}

variable_struct::variable_struct(const variable_t *object) {
  if (object) {
    data = object->data;
    type = object->type;
  }
}
  
variable_struct::variable_struct(bool value) {
  type = kVariableTypeBool;
  data = value ? "1" : "0";
}
 
variable_struct::variable_struct(int32_t value) {
  type = kVariableTypeInt32;
  char temp[128] = {0};
  snprintf(temp, sizeof(temp) - 1, "%" PRIu32 "", value);
  data = temp;
}
 
variable_struct::variable_struct(uint32_t value) {
  type = kVariableTypeUint32;
  char temp[128] = {0};
  snprintf(temp, sizeof(temp) - 1, "%" PRIu32 "", value);
  data = temp;
}
  
variable_struct::variable_struct(int16_t value) {
  type = kVariableTypeInt16;
  char temp[128] = {0};
  snprintf(temp, sizeof(temp) - 1, "%" PRId16 "", value);
  data = temp;
}

variable_struct::variable_struct(uint16_t value) {
  type = kVariableTypeUint16;
  char temp[128] = {0};
  snprintf(temp, sizeof(temp) - 1, "%" PRIu16 "", value);
  data = temp;
}
  
variable_struct::variable_struct(int8_t value) {
  type = kVariableTypeInt8;
  char temp[128] = {0};
  snprintf(temp, sizeof(temp) - 1, "%" PRId8 "", value);
  data = temp;
}

variable_struct::variable_struct(uint8_t value) {
  type = kVariableTypeUint8;
  char temp[128] = {0};
  snprintf(temp, sizeof(temp) - 1, "%" PRIu8 "", value);
  data = temp;
}

variable_struct::variable_struct(int64_t value) {
  type = kVariableTypeInt64;
  char temp[128] = {0};
  snprintf(temp, sizeof(temp) - 1, "%" PRId64 "", value);
  data = temp;
}
 
variable_struct::variable_struct(uint64_t value) {
  type = kVariableTypeUint64;
  char temp[128] = {0};
  snprintf(temp, sizeof(temp) - 1, "%" PRId64 "", value);
  data = temp;
}

variable_struct::variable_struct(float value) {
  type = kVariableTypeFloat;
  char temp[128] = {0};
  snprintf(temp, sizeof(temp) - 1, "%f", value);
  data = temp;
}

variable_struct::variable_struct(double value) {
  type = kVariableTypeDouble;
  char temp[128] = {0};
  snprintf(temp, sizeof(temp) - 1, "%f", value);
  data = temp;
}
  
variable_struct::variable_struct(std::string value) {
  type = kVariableTypeString;
  data = value;
}
  
variable_struct::variable_struct(const char *value) {
  type = kVariableTypeString;
  data = value;
}
 
bool variable_struct::_bool() const {
  bool result = int64() != 0;
  return result;
}
 
int32_t variable_struct::int32() const {
  int32_t result = static_cast<int32_t>(int64());
  return result;
}

uint32_t variable_struct::uint32() const {
  uint32_t result = static_cast<uint32_t>(int64());
  return result;
}

int16_t variable_struct::int16() const {
  int16_t result = static_cast<int16_t>(int64());
  return result;
}
  
uint16_t variable_struct::uint16() const {
  uint16_t result = static_cast<uint16_t>(int64());
  return result;
}

int8_t variable_struct::int8() const {
  int8_t result = static_cast<int8_t>(int64());
  return result;
}
  
uint8_t variable_struct::uint8() const {
  uint8_t result = static_cast<uint8_t>(int64());
  return result;
}
  
int64_t variable_struct::int64() const {
  char *endpointer = nullptr;
  int64_t result = strtoint64(data.c_str(), &endpointer, 10);
  return result;
}

uint64_t variable_struct::uint64() const {
  uint64_t result = static_cast<uint64_t>(int64());
  return result;
}
  
float variable_struct::_float() const {
  float result = static_cast<float>(_double());
  return result;
}
  
double variable_struct::_double() const {
  double result = atof(data.c_str());
  return result;
}
  
const char *variable_struct::string() const {
  return data.c_str();
}

variable_t &variable_struct::operator = (const variable_t &object) {
  data = object.data;
  type = object.type;
  return *this;
}
  
variable_t *variable_struct::operator = (const variable_t *object) {
  if (object) {
    data = object->data;
    type = object->type;
  }
  return this;
}
  
variable_t &variable_struct::operator = (int32_t value) {
  type = kVariableTypeInt32;
  char temp[128] = {0};
  snprintf(temp, sizeof(temp) - 1, "%" PRId32 "", value);
  data = temp;
  return *this;
}
  
variable_t &variable_struct::operator = (uint32_t value) {
  type = kVariableTypeUint32;
  char temp[128] = {0};
  snprintf(temp, sizeof(temp) - 1, "%" PRIu32 "", value);
  data = temp;
  return *this;
}
  
variable_t &variable_struct::operator = (int16_t value) {
  type = kVariableTypeInt16;
  char temp[128] = {0};
  snprintf(temp, sizeof(temp) - 1, "%" PRId16 "", value);
  data = temp;
  return *this;
}
  
variable_t &variable_struct::operator = (uint16_t value) {
  type = kVariableTypeUint16;
  char temp[128] = {0};
  snprintf(temp, sizeof(temp) - 1, "%" PRIu16 "", value);
  data = temp;
  return *this;
}
  
variable_t &variable_struct::operator = (int8_t value) {
  type = kVariableTypeInt8;
  char temp[128] = {0};
  snprintf(temp, sizeof(temp) - 1, "%" PRId8 "", value);
  data = temp;
  return *this;
}
  
variable_t &variable_struct::operator = (uint8_t value) {
  type = kVariableTypeUint8;
  char temp[128] = {0};
  snprintf(temp, sizeof(temp) - 1, "%" PRIu8 "", value);
  data = temp;
  return *this;
}
variable_t &variable_struct::operator = (bool value) {
  type = kVariableTypeBool;
  data = value ? "1" : "0";
  return *this;
}
 
variable_t &variable_struct::operator = (int64_t value) {
  type = kVariableTypeInt64;
  char temp[128] = {0};
  snprintf(temp, sizeof(temp) - 1, "%" PRId64 "", value);
  data = temp;
  return *this;
  return *this;
}
  
variable_t &variable_struct::operator = (uint64_t value) {
  type = kVariableTypeUint64;
  char temp[128] = {0};
  snprintf(temp, sizeof(temp) - 1, "%" PRIu64 "", value);
  data = temp;
  return *this;
  return *this;
}

variable_t &variable_struct::operator = (float value) {
  type = kVariableTypeFloat;
  char temp[128] = {0};
  snprintf(temp, sizeof(temp) - 1, "%f", value);
  data = temp;
  return *this;
  return *this;
}

variable_t &variable_struct::operator = (double value) {
  type = kVariableTypeDouble;
  char temp[128] = {0};
  snprintf(temp, sizeof(temp) - 1, "%f", value);
  data = temp;
  return *this;
  return *this;
}
  
variable_t &variable_struct::operator = (std::string value) {
  type = kVariableTypeString;
  data = value;
  return *this;
  return *this;
}
  
variable_t &variable_struct::operator = (const char *value) {
  type = kVariableTypeString;
  data = value;
  return *this;
  return *this;
}

variable_t &variable_struct::operator += (const variable_t &object) {
  switch (type) {
    case kVariableTypeInt32:
    *this += object.int32();
    break;
    case kVariableTypeUint32:
    *this += object.uint32();
    break;
    case kVariableTypeInt16:
    *this += object.int16();
    break;
    case kVariableTypeUint16:
    *this += object.uint16();
    break;
    case kVariableTypeInt8:
    *this += object.int8();
    break;
    case kVariableTypeUint8:
    *this += object.uint8();
    break;
    case kVariableTypeInt64:
    *this += object.int64();
    break;
    case kVariableTypeUint64:
    *this += object.uint64();
    break;
    case kVariableTypeFloat:
    *this += object._float();
    break;
    case kVariableTypeDouble:
    *this += object._double();
    break;
    default:
    *this += object.data;
    break;
  }
  return *this;
}
  
variable_t *variable_struct::operator += (const variable_t *object) {
  if (object) *this += *object;
  return this;
}

variable_t &variable_struct::operator += (int32_t value) {
  int32_t finalvalue = int32() + value;
  *this = finalvalue;
  return *this;
}
  
variable_t &variable_struct::operator += (uint32_t value) {
  uint32_t finalvalue = uint32() + value;
  *this = finalvalue;
  return *this;
}
 
variable_t &variable_struct::operator += (int16_t value) {
  int16_t finalvalue = int16() + value;
  *this = finalvalue;
  return *this;
}
  
variable_t &variable_struct::operator += (uint16_t value) {
  uint16_t finalvalue = uint16() + value;
  *this = finalvalue;
  return *this;
}
  
variable_t &variable_struct::operator += (int8_t value) {
  int8_t finalvalue = int8() + value;
  *this = finalvalue;
  return *this;
}
  
variable_t &variable_struct::operator += (uint8_t value) {
  uint8_t finalvalue = uint8() + value;
  *this = finalvalue;
  return *this;
}
  
variable_t &variable_struct::operator += (int64_t value) {
  int64_t finalvalue = int64() + value;
  *this = finalvalue;
  return *this;
}
  
variable_t &variable_struct::operator += (uint64_t value) {
  uint64_t finalvalue = uint64() + value;
  *this = finalvalue;
  return *this;
}

variable_t &variable_struct::operator += (float value) {
  float finalvalue = value + value;
  *this = finalvalue;
  return *this;
}
  
variable_t &variable_struct::operator += (double value) {
  double finalvalue = _double() + value;
  *this = finalvalue;
  return *this;
}
  
variable_t &variable_struct::operator += (std::string value) {
  data += value;
  return *this;
}
  
variable_t &variable_struct::operator += (const char *value) {
  data += value;
  return *this;
}

variable_t &variable_struct::operator -= (const variable_t &object) {
  switch (type) {
    case kVariableTypeInt32:
    *this -= object.int32();
    break;
    case kVariableTypeUint32:
    *this -= object.uint32();
    break;
    case kVariableTypeInt16:
    *this -= object.int16();
    break;
    case kVariableTypeUint16:
    *this -= object.uint16();
    break;
    case kVariableTypeInt8:
    *this -= object.int8();
    break;
    case kVariableTypeUint8:
    *this -= object.uint8();
    break;
    case kVariableTypeInt64:
    *this -= object.int64();
    break;
    case kVariableTypeUint64:
    *this -= object.uint64();
    break;
    case kVariableTypeFloat:
    *this -= object._float();
    break;
    case kVariableTypeDouble:
    *this -= object._double();
    break;
    default:
    *this -= object.data;
    break;
  }
  return *this;
}
  
variable_t *variable_struct::operator -= (const variable_t *object) {
  if (object) *this -= *object;
  return this;
}

variable_t &variable_struct::operator -= (int32_t value) {
  int32_t finalvalue = int32() - value;
  *this = finalvalue;
  return *this;
}
  
variable_t &variable_struct::operator -= (uint32_t value) {
  uint32_t finalvalue = uint32() - value;
  *this = finalvalue;
  return *this;
}
 
variable_t &variable_struct::operator -= (int16_t value) {
  int16_t finalvalue = int16() - value;
  *this = finalvalue;
  return *this;
}
  
variable_t &variable_struct::operator -= (uint16_t value) {
  uint16_t finalvalue = uint16() - value;
  *this = finalvalue;
  return *this;
}
  
variable_t &variable_struct::operator -= (int8_t value) {
  int8_t finalvalue = int8() - value;
  *this = finalvalue;
  return *this;
}
  
variable_t &variable_struct::operator -= (uint8_t value) {
  uint8_t finalvalue = uint8() - value;
  *this = finalvalue;
  return *this;
}
  
variable_t &variable_struct::operator -= (int64_t value) {
  int64_t finalvalue = int64() - value;
  *this = finalvalue;
  return *this;
}
  
variable_t &variable_struct::operator -= (uint64_t value) {
  uint64_t finalvalue = uint64() - value;
  *this = finalvalue;
  return *this;
}

variable_t &variable_struct::operator -= (float value) {
  float finalvalue = _float() - value;
  *this = finalvalue;
  return *this;
}
  
variable_t &variable_struct::operator -= (double value) {
  double finalvalue = _double() - value;
  *this = finalvalue;
  return *this;
}
  
variable_t &variable_struct::operator *= (const variable_t &object) {
  switch (type) {
    case kVariableTypeInt32:
    *this *= object.int32();
    break;
    case kVariableTypeUint32:
    *this *= object.uint32();
    break;
    case kVariableTypeInt16:
    *this *= object.int16();
    break;
    case kVariableTypeUint16:
    *this *= object.uint16();
    break;
    case kVariableTypeInt8:
    *this *= object.int8();
    break;
    case kVariableTypeUint8:
    *this *= object.uint8();
    break;
    case kVariableTypeInt64:
    *this *= object.int64();
    break;
    case kVariableTypeUint64:
    *this *= object.uint64();
    break;
    case kVariableTypeFloat:
    *this *= object._float();
    break;
    case kVariableTypeDouble:
    *this *= object._double();
    break;
    default:
    break;
  }
  return *this;
}
  
variable_t *variable_struct::operator *= (const variable_t *object) {
  if (object) *this *= *object;
  return this;
}

variable_t &variable_struct::operator *= (int32_t value) {
  int32_t finalvalue = int32() * value;
  *this = finalvalue;
  return *this;
}
  
variable_t &variable_struct::operator *= (uint32_t value) {
  uint32_t finalvalue = uint32() * value;
  *this = finalvalue;
  return *this;
}
 
variable_t &variable_struct::operator *= (int16_t value) {
  int16_t finalvalue = int16() * value;
  *this = finalvalue;
  return *this;
}
  
variable_t &variable_struct::operator *= (uint16_t value) {
  uint16_t finalvalue = uint16() * value;
  *this = finalvalue;
  return *this;
}
  
variable_t &variable_struct::operator *= (int8_t value) {
  int8_t finalvalue = int8() * value;
  *this = finalvalue;
  return *this;
}
  
variable_t &variable_struct::operator *= (uint8_t value) {
  uint8_t finalvalue = uint8() * value;
  *this = finalvalue;
  return *this;
}
  
variable_t &variable_struct::operator *= (int64_t value) {
  int64_t finalvalue = int64() * value;
  *this = finalvalue;
  return *this;
}
  
variable_t &variable_struct::operator *= (uint64_t value) {
  uint64_t finalvalue = uint64() * value;
  *this = finalvalue;
  return *this;
}

variable_t &variable_struct::operator *= (float value) {
  float finalvalue = _float() * value;
  *this = finalvalue;
  return *this;
}
  
variable_t &variable_struct::operator *= (double value) {
  double finalvalue = _double() * value;
  *this = finalvalue;
  return *this;
}

variable_t &variable_struct::operator /= (const variable_t &object) {
  switch (type) {
    case kVariableTypeInt32:
    *this /= object.int32();
    break;
    case kVariableTypeUint32:
    *this /= object.uint32();
    break;
    case kVariableTypeInt16:
    *this /= object.int16();
    break;
    case kVariableTypeUint16:
    *this /= object.uint16();
    break;
    case kVariableTypeInt8:
    *this /= object.int8();
    break;
    case kVariableTypeUint8:
    *this /= object.uint8();
    break;
    case kVariableTypeInt64:
    *this /= object.int64();
    break;
    case kVariableTypeUint64:
    *this /= object.uint64();
    break;
    case kVariableTypeFloat:
    *this /= object._float();
    break;
    case kVariableTypeDouble:
    *this /= object._double();
    break;
    default:
    break;
  }
  return *this;
}

variable_t *variable_struct::operator /= (const variable_t *object) {
  if (object) *this /= *object;
  return this;
}

variable_t &variable_struct::operator /= (int32_t value) {
  int32_t finalvalue = int32() / value;
  *this = finalvalue;
  return *this;
}
  
variable_t &variable_struct::operator /= (uint32_t value) {
  uint32_t finalvalue = uint32() / value;
  *this = finalvalue;
  return *this;
}
 
variable_t &variable_struct::operator /= (int16_t value) {
  int16_t finalvalue = int16() / value;
  *this = finalvalue;
  return *this;
}
  
variable_t &variable_struct::operator /= (uint16_t value) {
  uint16_t finalvalue = uint16() / value;
  *this = finalvalue;
  return *this;
}
  
variable_t &variable_struct::operator /= (int8_t value) {
  int8_t finalvalue = int8() / value;
  *this = finalvalue;
  return *this;
}
  
variable_t &variable_struct::operator /= (uint8_t value) {
  uint8_t finalvalue = uint8() / value;
  *this = finalvalue;
  return *this;
}
  
variable_t &variable_struct::operator /= (int64_t value) {
  int64_t finalvalue = int64() / value;
  *this = finalvalue;
  return *this;
}
  
variable_t &variable_struct::operator /= (uint64_t value) {
  uint64_t finalvalue = uint64() / value;
  *this = finalvalue;
  return *this;
}

variable_t &variable_struct::operator /= (float value) {
  float finalvalue = _float() / value;
  *this = finalvalue;
  return *this;
}
  
variable_t &variable_struct::operator /= (double value) {
  double finalvalue = _double() / value;
  *this = finalvalue;
  return *this;
}  

variable_t &variable_struct::operator ++ () {
  *this += 1;
  return *this;
}
  
variable_t &variable_struct::operator -- () {
  *this -= 1;
  return *this;
}

variable_t &variable_struct::operator ++ (int32_t) {
  *this += 1;
  return *this;
}
  
variable_t &variable_struct::operator -- (int32_t) {
  *this -= 1;
  return *this;
}

bool variable_struct::operator == (const variable_t &object) const {
  return data == object.data;
}
  
bool variable_struct::operator == (const variable_t *object) const {
  if (is_null(object)) return false;
  return data == object->data;
}

bool variable_struct::operator == (const int32_t value) const {
  variable_t variable = value;
  return *this == variable;
}

bool variable_struct::operator == (const uint32_t value) const {
  variable_t variable = value;
  return *this == variable;
}
  
bool variable_struct::operator == (const int16_t value) const {
  variable_t variable = value;
  return *this == variable;
}

bool variable_struct::operator == (const uint16_t value) const {
  variable_t variable = value;
  return *this == variable;
}
  
bool variable_struct::operator == (const int8_t value) const {
  variable_t variable = value;
  return *this == variable;
}
  
bool variable_struct::operator == (const uint8_t value) const {
  variable_t variable = value;
  return *this == variable;
}

bool variable_struct::operator == (const bool value) const {
  variable_t variable = value;
  return *this == variable;
}
  
bool variable_struct::operator == (const int64_t value) const {
  variable_t variable = value;
  return *this == variable;
}

bool variable_struct::operator == (const uint64_t value) const {
  variable_t variable = value;
  return *this == variable;
}
  
bool variable_struct::operator == (const float value) const {
  variable_t variable = value;
  return *this == variable;
}

bool variable_struct::operator == (const double value) const {
  variable_t variable = value;
  return *this == variable;
}

bool variable_struct::operator == (const std::string value) const {
  variable_t variable = value;
  return *this == variable;
}

bool variable_struct::operator == (const char *value) const {
  variable_t variable = value;
  return *this == variable;
}

bool variable_struct::operator != (const variable_t &object) const {
  return !(data == object.data);
}
  
bool variable_struct::operator != (const variable_t *object) const {
  if (is_null(object)) return false;
  return !(data == object->data);
}

bool variable_struct::operator != (const int32_t value) const {
  variable_t variable = value;
  return !(*this == variable);
}

bool variable_struct::operator != (const uint32_t value) const {
  variable_t variable = value;
  return !(*this == variable);
}
  
bool variable_struct::operator != (const int16_t value) const {
  variable_t variable = value;
  return !(*this == variable);
}

bool variable_struct::operator != (const uint16_t value) const {
  variable_t variable = value;
  return !(*this == variable);
}
  
bool variable_struct::operator != (const int8_t value) const {
  variable_t variable = value;
  return !(*this == variable);
}
  
bool variable_struct::operator != (const uint8_t value) const {
  variable_t variable = value;
  return !(*this == variable);
}

bool variable_struct::operator != (const bool value) const {
  variable_t variable = value;
  return !(*this == variable);
}
  
bool variable_struct::operator != (const int64_t value) const {
  variable_t variable = value;
  return !(*this == variable);
}

bool variable_struct::operator != (const uint64_t value) const {
  variable_t variable = value;
  return !(*this == variable);
}
  
bool variable_struct::operator != (const float value) const {
  variable_t variable = value;
  return !(*this == variable);
}

bool variable_struct::operator != (const double value) const {
  variable_t variable = value;
  return !(*this == variable);
}

bool variable_struct::operator != (const std::string value) const {
  variable_t variable = value;
  return !(*this == variable);
}

bool variable_struct::operator != (const char *value) const {
  variable_t variable = value;
  return !(*this == variable);
}
bool variable_struct::operator < (const variable_t &object) const {
  bool result = data < object.data;
  return result;
}

bool variable_struct::operator < (const variable_t *object) const {
  bool result = false;
  if (object) result = data < object->data;
  return result;
}
  
bool variable_struct::operator < (const int32_t value) const {
  variable_t variable = value;
  return *this < variable;
}
  
bool variable_struct::operator < (const uint32_t value) const {
  variable_t variable = value;
  return *this < variable;
}
  
bool variable_struct::operator < (const int16_t value) const {
  variable_t variable = value;
  return *this < variable;
}
  
bool variable_struct::operator < (const uint16_t value) const {
  variable_t variable = value;
  return *this < variable;
}
  
bool variable_struct::operator < (const int8_t value) const {
  variable_t variable = value;
  return *this < variable;
}
  
bool variable_struct::operator < (const uint8_t value) const {
  variable_t variable = value;
  return *this < variable;
}
  
bool variable_struct::operator < (const int64_t value) const {
  variable_t variable = value;
  return *this < variable;
}
  
bool variable_struct::operator < (const uint64_t value) const {
  variable_t variable = value;
  return *this < variable;
}
  
bool variable_struct::operator < (const float value) const {
  variable_t variable = value;
  return *this < variable;
}
  
bool variable_struct::operator < (const double value) const {
  variable_t variable = value;
  return *this < variable;
}
  
bool variable_struct::operator < (const std::string value) const {
  variable_t variable = value;
  return *this < variable;
}
  
bool variable_struct::operator < (const char *value) const {
  variable_t variable = value;
  return *this < variable;
}
  
bool variable_struct::operator > (const variable_t &object) const {
  return data > object.data;
}

bool variable_struct::operator > (const variable_t *object) const {
  if (is_null(object)) return true;
  return data > object->data;
}
  
bool variable_struct::operator > (const int32_t value) const {
  variable_t variable = value;
  return *this > variable;
}
  
bool variable_struct::operator > (const uint32_t value) const {
  variable_t variable = value;
  return *this > variable;
}

bool variable_struct::operator > (const int16_t value) const {
  variable_t variable = value;
  return *this > variable;
}
  
bool variable_struct::operator > (const uint16_t value) const {
  variable_t variable = value;
  return *this > variable;
}
  
bool variable_struct::operator > (const int8_t value) const {
  variable_t variable = value;
  return *this > variable;
}
  
bool variable_struct::operator > (const uint8_t value) const {
  variable_t variable = value;
  return *this > variable;
}
  
bool variable_struct::operator > (const int64_t value) const {
  variable_t variable = value;
  return *this > variable;
}
  
bool variable_struct::operator > (const uint64_t value) const {
  variable_t variable = value;
  return *this > variable;
}
  
bool variable_struct::operator > (const float value) const {
  variable_t variable = value;
  return *this > variable;
}
  
bool variable_struct::operator > (const double value) const {
  variable_t variable = value;
  return *this > variable;
}
  
bool variable_struct::operator > (const std::string value) const {
  variable_t variable = value;
  return *this > variable;
}
  
bool variable_struct::operator > (const char *value) const {
  variable_t variable = value;
  return *this > variable;
}

}; //namespace type

}; //namespace pf_basic
