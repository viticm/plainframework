#include "structure/object.h"
#include "structure/manager/objectsingle.h"

namespace structure {

namespace manager {

ObjectSingle::ObjectSingle() {
  __ENTER_FUNCTION
    objects_ = NULL;
    count_ = 0;
    length_ = 0;
    logicinterval_ = 0;
    current_logicinterval_ = 0;
    scene_ = NULL;
  __LEAVE_FUNCTION
}

ObjectSingle::~ObjectSingle() {
  __ENTER_FUNCTION
    destroy();
    Assert(NULL == objects_ && "objects_ not free");
  __LEAVE_FUNCTION
}

bool ObjectSingle::init(objectsingle_manager_init_t *initdata) {
  __ENTER_FUNCTION
    Assert(NULL == objects_ && "objects_ not free");
    if (NULL == initdata || 0 == initdata->length || NULL == initdata->scene) {
      return false;
    }
    count_ = 0;
    length_ = initdata->length;
    objects_ = new structure::Object * [length_];
    memset(objects_, 0, sizeof(structure::Object *) * length_);
    logicinterval_ = initdata->logicinterval;
    current_logicinterval_ = initdata->logicinterval;
    scene_ = initdata->scene;
    logictimer_.start(initdata->logicinterval, initdata->currenttime);
    return true;
  __LEAVE_FUNCTION
    return false;
}

void ObjectSingle::destroy() {
  __ENTER_FUNCTION
    SAFE_DELETE_ARRAY(objects_);
    count_ = 0;
    length_ = 0;
    logicinterval_ = 0;
    current_logicinterval_ = 0;
    scene_ = NULL;
    logictimer_.cleanup();
  __LEAVE_FUNCTION
}

void ObjectSingle::reset() {
  __ENTER_FUNCTION
    count_ = 0;
    memset(objects_, 0, sizeof(structure::Object *) * length_);
  __LEAVE_FUNCTION
}

void ObjectSingle::set_loadfactor(float factor) {
  __ENTER_FUNCTION
    current_logicinterval_ = static_cast<uint32_t>(factor * logicinterval_);
    logictimer_.set_termtime(current_logicinterval_);
  __LEAVE_FUNCTION
}

bool ObjectSingle::heartbeat(uint32_t time) {
  __ENTER_FUNCTION
    if (!logictimer_.counting(time)) return true;
    uint32_t i;
    bool result = true;
    for (i = 0; i < count_; ++i) {
      if (objects_[i] != NULL) {
        if (objects_[i]->isactive()) {
          result = objects_[i]->heartbeat(time);
        } else {
          result = objects_[i]->heartbeat_outscene(time);
        }
      }
    }
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool ObjectSingle::add(structure::Object *object) {
  __ENTER_FUNCTION
    if (NULL == object) return false;
    if (count_ >= length_) {
      uint32_t newsize;
      newsize = length_ * 2;
      bool result = resize(newsize);
      if (!result) {
        return false;
      }
    }
    objects_[count_] = object;
    object->set_singlemanager_index(count_);
    ++count_;
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool ObjectSingle::remove(structure::Object *object) {
  __ENTER_FUNCTION
    if (NULL == object) return false;
    uint32_t index = object->get_singlemanager_index();
    if (index >= count_) return false;
    --count_;
    objects_[count_]->set_singlemanager_index(index);
    objects_[index] = objects_[count_];
    objects_[count_] = NULL;
    return true;
  __LEAVE_FUNCTION
    return false;
}

uint32_t ObjectSingle::getcount() const {
  return count_;
}

uint32_t ObjectSingle::getlength() const {
  return length_;
}

void ObjectSingle::setscene(void *scene) {
  scene_ = scene;
}

void *ObjectSingle::getscene() {
  return scene_;
}

structure::Object *ObjectSingle::get(uint32_t index) {
  __ENTER_FUNCTION
    structure::Object *object = NULL;
    if (index < count_) object = objects_[index];
    return object;
  __LEAVE_FUNCTION
    return NULL;
}

bool ObjectSingle::resize(uint32_t size) {
  __ENTER_FUNCTION
    structure::Object **save = objects_;
    objects_ = new structure::Object * [size];
    if (save != NULL)
      memcpy(objects_, save, sizeof(structure::Object *) * size);
      memset(&objects_[count_], 
           0, 
           sizeof(structure::Object * ) * (size - count_));
    length_ = size;
    SAFE_DELETE_ARRAY(save);
    return true;
  __LEAVE_FUNCTION
    return false;
}

} //namespace manager

} //namespace structure
