#include "structure/object.h"
#include "structure/manager/object.h"

namespace structure {

namespace manager {

Object::Object() {
  __ENTER_FUNCTION
    memset(objects_, 0, sizeof(objects_));
    normalposition_ = 0;
    humanposition_ = STRUCTURE_MANAGER_OBJECT_HUMAN_START;
  __LEAVE_FUNCTION
}

Object::~Object() {
  __ENTER_FUNCTION
    uint32_t i;
    for (i = 0; i < STRUCTURE_MANAGER_OBJECT_IDSIZE_MAX; ++i)
      Assert(NULL == objects_[i] && "object list not free");
  __LEAVE_FUNCTION
}

bool Object::init(const objectmanager_init_t *initdata) {
  __ENTER_FUNCTION
    memset(objects_, 0, sizeof(objects_));
    normalposition_ = 0;
    humanposition_ = STRUCTURE_MANAGER_OBJECT_HUMAN_START;
    return true;
  __LEAVE_FUNCTION
    return false;
}
   
void Object::destroy() {
  __ENTER_FUNCTION
    memset(objects_, 0, sizeof(objects_));
    normalposition_ = 0;
    humanposition_ = STRUCTURE_MANAGER_OBJECT_HUMAN_START;
  __LEAVE_FUNCTION
}

bool Object::add(structure::Object *object, int8_t idtype) {
  __ENTER_FUNCTION
    if (NULL == object) return false;
    if (kObjectIdTypeNormal == idtype) {
      objects_[normalposition_] = object;
      object->setid(normalposition_);
      for (int32_t i = 0; i < STRUCTURE_MANAGER_OBJECT_HUMAN_START; ++i) {
        ++normalposition_;
        if (normalposition_ >= STRUCTURE_MANAGER_OBJECT_HUMAN_START)
          normalposition_ = 0;
        if (NULL == objects_[normalposition_]) break;
      }
    } else if (kObjectIdTypeHuman == idtype) {
      objects_[humanposition_] = object;
      object->setid(humanposition_);
      for (int32_t i = STRUCTURE_MANAGER_OBJECT_HUMAN_START; 
           i < STRUCTURE_MANAGER_OBJECT_IDSIZE_MAX; 
           ++i) {
        ++humanposition_;
        if (humanposition_ >= STRUCTURE_MANAGER_OBJECT_IDSIZE_MAX)
          humanposition_ = 0;
        if (NULL == objects_[humanposition_]) break;
      }
    } else {
      return false;
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Object::remove(objectid_t id) {
  __ENTER_FUNCTION
    if (static_cast<objectid_t>(ID_INVALID) == id || 
        id >= STRUCTURE_MANAGER_OBJECT_IDSIZE_MAX) 
      return false;
    if (NULL == objects_[id]) return false;
    objects_[id]->setid(ID_INVALID);
    objects_[id] = NULL;
    return true;
  __LEAVE_FUNCTION
    return false;
}
   
structure::Object *Object::get(objectid_t id) {
  __ENTER_FUNCTION
    if (static_cast<objectid_t>(ID_INVALID) == id || 
        id >= STRUCTURE_MANAGER_OBJECT_IDSIZE_MAX)
      return NULL;
    structure::Object *object = objects_[id];
    return object;
  __LEAVE_FUNCTION
    return NULL;
}
   
int32_t Object::get_normalcount() const {
  __ENTER_FUNCTION
    int32_t count = 0;
    for (int32_t i = 0; i < STRUCTURE_MANAGER_OBJECT_HUMAN_START; ++i) {
      if (objects_[i] != NULL) ++count;
    }
    return count;
  __LEAVE_FUNCTION
    return 0;
}

int32_t Object::get_humancount() const {
  __ENTER_FUNCTION
    int32_t count = 0;
    for (int32_t i = STRUCTURE_MANAGER_OBJECT_HUMAN_START; 
         i < STRUCTURE_MANAGER_OBJECT_IDSIZE_MAX; 
         ++i) {
      if (objects_[i] != NULL) ++count;
    }
    return count;
  __LEAVE_FUNCTION
    return 0;
}

object_idtype_t Object::get_idtype(objectid_t id) const {
  __ENTER_FUNCTION
    if (id >= STRUCTURE_MANAGER_OBJECT_HUMAN_START)
      return kObjectIdTypeHuman;
    else
      return kObjectIdTypeNormal;
  __LEAVE_FUNCTION
    return kObjectIdTypeInvalid;
}

} //namespace manager

} //namespace structure
