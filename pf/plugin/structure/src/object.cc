#include "structure/object.h"

namespace structure {

Object::Object() {
  __ENTER_FUNCTION
    id_ = ID_INVALID;
    type_ = kObjectTypeInvalid;
    poolid_ = UINT_MAX;
    singlemanager_index_ = UINT_MAX;
    viewgroup_ = 0;
    isactive_ = false;
    logiccount_ = 0;
    second_logiccount_ = 0;
    move_logiccount_ = 0;
    position_.clear();
    listnode_ = new objectlist_node_t(this);
    direction_ = .0f;
    nowtime_ = 0;
    lasttime_ = 0;
    createtime_ = 0;
    gridid_ = ID_INVALID;
    scene_ = NULL;
    netconnection_ = NULL;
    lockedtarget_ = ID_INVALID;
    guid_ = ID_INVALID;
  __LEAVE_FUNCTION
}

Object::~Object() {
  __ENTER_FUNCTION
    SAFE_DELETE(listnode_);
  __LEAVE_FUNCTION
}

bool Object::init(object_t *object) {
  __ENTER_FUNCTION
    if (NULL == object) return false;
    position_ = object->position;
    direction_ = object->direction;
    updategrid();
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Object::heartbeat(uint32_t time) {
  __ENTER_FUNCTION
    updatetime(time);
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Object::heartbeat_outscene(uint32_t time) {
  __ENTER_FUNCTION
    updatetime(time);
    return true;
  __LEAVE_FUNCTION
    return false;
}

void Object::update_viewcharacter(int32_t old_viewgroup) {
  USE_PARAM(old_viewgroup);
}

int8_t Object::gettype() const {
  return type_;
}

void Object::setguid(guid_t guid) {
  guid_ = guid;
}

guid_t Object::getguid() const {
  return guid_;
}

bool Object::updategrid() {
  return true;
}

void Object::clear() {
  __ENTER_FUNCTION
    isactive_ = false;
    Assert(NULL == listnode_->next && NULL == listnode_->prev);
    listnode_->next = NULL;
    listnode_->prev = NULL;
    position_.clear();
    direction_ = .0f;
    gridid_ = ID_INVALID;
    move_logiccount_ = 0;
  __LEAVE_FUNCTION
}

void Object::on_enterscene() {
  __ENTER_FUNCTION
    nowtime_ = 0;
    lasttime_ = 0;
    createtime_ = 0;
  __LEAVE_FUNCTION
}
   
void Object::on_leavescene() {
  __ENTER_FUNCTION
    nowtime_ = 0;
    lasttime_ = 0;
    createtime_ = 0;
  __LEAVE_FUNCTION
}

void Object::on_enterscene_success() {
  //do nothing -- implement at child class
}

void Object::on_register_togrid() {
  //do nothing -- implement at child class
}

void Object::on_unregister_fromgrid() {
  //do nothing -- implement at child class
}

objectid_t Object::getid() const {
  return id_;
}

void Object::setid(objectid_t id) {
  id_ = id;
}

int64_t Object::get_uniqueid() const {
  return 0;
}
   
void Object::set_poolid(uint32_t id) {
  poolid_ = id;
}

uint32_t Object::get_poolid() const {
  return poolid_;
}

void Object::set_singlemanager_index(uint32_t index) {
  singlemanager_index_ = index;
}
  
uint32_t Object::get_singlemanager_index() const {
  return singlemanager_index_;
}

uint32_t Object::get_logictime() const {
  uint32_t time = nowtime_ - lasttime_;
  return time;
}
   
uint32_t Object::get_lasttime() const {
  return lasttime_;
}

uint32_t Object::get_nowtime() const {
  return nowtime_;
}

uint32_t Object::get_createtime() const {
  return createtime_;
}

void Object::set_viewgroup(int32_t viewgroup) {
  viewgroup_ = viewgroup;
}

int32_t Object::get_viewgroup() const {
  return viewgroup_;
}
   
objectlist_node_t *Object::get_objectnode() {
  return listnode_;
}

bool Object::can_viewme(const Object *object) {
  __ENTER_FUNCTION
    if (NULL ==  object) return false;
    if (object->getid() == getid()) return true;
    bool result = object->get_viewgroup() == get_viewgroup();
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Object::can_viewhim(const Object *object) {
  __ENTER_FUNCTION
    if (NULL ==  object) return false;
    if (object->getid() == getid()) return true;
    bool result = object->get_viewgroup() == get_viewgroup();
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Object::can_prev_viewme(const Object *object, int32_t old_viewgroup) {
  __ENTER_FUNCTION
    if (NULL ==  object) return false;
    if (object->getid() == getid()) return true;
    bool result = object->get_viewgroup() == old_viewgroup;
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Object::can_prev_viewhim(const Object *object, int32_t old_viewgroup) {
  __ENTER_FUNCTION
    if (NULL ==  object) return false;
    if (object->getid() == getid()) return true;
    bool result = object->get_viewgroup() == old_viewgroup;
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Object::can_stealth_viewme(const Object *object) {
  __ENTER_FUNCTION
    if (NULL ==  object) return false;
    if (object->getid() == getid()) return true;
    bool result = object->get_viewgroup() == get_viewgroup();
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Object::can_stealth_viewhim(const Object *object) {
  __ENTER_FUNCTION
    if (NULL ==  object) return false;
    if (object->getid() == getid()) return true;
    bool result = object->get_viewgroup() == get_viewgroup();
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Object::isactive() const {
  return isactive_;
}
   
const position_t *Object::getposition() const {
  return &position_;
}

void Object::setposition(position_t *position) {
  position_ = *position;
}
   
void Object::setdirection(float direction) {
  direction_ = direction;
}
   
float Object::getdirection() const {
  return direction_;
}
   
void Object::setscene(void *scene) {
  scene_ = scene;
}
   
void *Object::getscene() {
  return scene_;
}
   
void Object::set_gridid(gridid_t id) {
  gridid_ = id;
}
   
gridid_t Object::get_gridid() const {
  return gridid_;
}

void Object::set_scriptid(scriptid_t id) {
  USE_PARAM(id);
}

scriptid_t Object::get_scriptid() const {
  return ID_INVALID;
}

bool Object::radius_invalid(float x1, 
                            float z1, 
                            float x2, 
                            float z2, 
                            float radius) {
  __ENTER_FUNCTION
    float x, z;
    x = x1 - x2;
    z = z1 - z2;
    bool result = (radius * radius) > x * x + z * z;
    return result;
  __LEAVE_FUNCTION
    return false;
}
  
bool Object::radius_invalid(const Object *object, float radius) {
  __ENTER_FUNCTION
    float x1 = position_.x;
    float z1 = position_.z;
    float x2 = object->getposition()->x;
    float z2 = object->getposition()->z;
    bool result = radius_invalid(x1, z1, x2, z2, radius);
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Object::radius_invalid(const position_t *position, float radius) {
  __ENTER_FUNCTION
    float x1 = position_.x;
    float z1 = position_.z;
    float x2 = position->x;
    float z2 = position->z;
    bool result = radius_invalid(x1, z1, x2, z2, radius);
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Object::in_pkarea() const {
  return false;
}

const campdata_t *Object::get_campdata() {
  return NULL;
}

uint8_t Object::get_logiccount() const {
  return logiccount_;
}
   
void Object::add_logiccount() {
  ++logiccount_;
  second_logiccount_ = 0;
}

uint8_t Object::get_second_logiccount() const {
  return second_logiccount_;
}
   
void Object::add_second_logiccount() {
  ++second_logiccount_;
}
   
int32_t Object::get_move_logiccount() const {
  return move_logiccount_;
}
   
void Object::add_move_logiccount() {
  ++move_logiccount_;
}

void Object::updatetime(uint32_t time) {
  __ENTER_FUNCTION
    if (0 == nowtime_) {
      createtime_ = time;
      lasttime_ = time;
    } else {
      lasttime_ = nowtime_;
    }
    nowtime_ = time;
  __LEAVE_FUNCTION
}

pf_net::connection::Base *Object::get_netconnection() {
  return netconnection_;
}

objectid_t Object::get_lockedtarget() const {
  return lockedtarget_;
}

void Object::set_dataid(uint32_t id) {
  dataid_ = id;
}
   
uint32_t Object::get_dataid() const {
  return dataid_;
}

void Object::process_talk() {
  //do nothing
}

void Object::set_aicontroller(void *aicontroller) {
  aicontroller_ = aicontroller;
}

void *Object::get_aicontroller() {
  return aicontroller_;
}

aidata_t *Object::get_aidata() {
  return aidata_;
}
   
void Object::set_aidata(aidata_t *data) {
  aidata_ = data;
}

}; //namespace structure
