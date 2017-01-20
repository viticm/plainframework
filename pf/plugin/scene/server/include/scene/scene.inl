/**
 * PAP Engine ( https://github.com/viticm/pap )
 * $Id scene.inl
 * @link https://github.com/viticm/pap for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm@126.com )
 * @license
 * @user viticm<viticm@126.com>
 * @date 2015/03/26 10:44
 * @uses scene class inline implements
 *       cn: 场景内联函数的实现
*/
#ifndef SCENE_SCENE_INL_
#define SCENE_SCENE_INL_

#include "pf/base/string.h"

namespace scene {

inline Grid *Scene::getgrid(gridid_t id) {
  Grid *result = ID_INVALID == id ? NULL : &grid_[id];
  return result;
}

inline void Scene::setload_map(const char *filename) {
  using namespace pf_base::string;
  memset(load_.map_filename, 0, sizeof(load_.map_filename));
  safecopy(load_.map_filename, filename, sizeof(load_.map_filename));
}

inline void Scene::setload_monster(const char *filename) {
  using namespace pf_base::string;
  memset(load_.monster_filename, 0, sizeof(load_.monster_filename));
  safecopy(load_.monster_filename, filename, sizeof(load_.monster_filename));
}

inline void Scene::setload_region(const char *filename) {
  using namespace pf_base::string;
  memset(load_.region_filename, 0, sizeof(load_.region_filename));
  safecopy(load_.region_filename, filename, sizeof(load_.region_filename));
}

inline void Scene::setfuben_sourcescene_id(const sceneid_t sceneid) {
  if (NULL == fuben_data_) return;
  fuben_data_->source_sceneid = sceneid;
}

inline void Scene::setfuben_teamleader(const guid_t guid) {
  if (NULL == fuben_data_) return;
  fuben_data_->teamleader = guid;
}

inline void Scene::setfuben_nouser_closetime(const uint32_t time) {
  if (NULL == fuben_data_) return;
  fuben_data_->nouser_closetime = time;
}

inline void Scene::setfuben_timer(const uint32_t timer) {
  if (NULL == fuben_data_) return;
  fuben_data_->scenetimer = timer;
}

inline void Scene::setfuben_param(const uint32_t index, const int32_t value) {
  if (NULL == fuben_data_) return;
  if (index >= SCENE_FUBEN_DATA_PARAM_MAX) {
    Assert(false && "out range the index");
    return;
  }
  fuben_data_->param[index] = value;
}

inline sceneid_t Scene::getfuben_sourcescene_id() {
  sceneid_t id = ID_INVALID;
  if (fuben_data_) id = fuben_data_->source_sceneid;
  return id;
}

inline guid_t Scene::getfuben_teamleader() {
  guid_t id = ID_INVALID;
  if (fuben_data_) id = fuben_data_->teamleader;
  return id;
}

inline uint32_t Scene::getfuben_nouser_closetime() {
  uint32_t time = 0;
  if (fuben_data_) time = fuben_data_->nouser_closetime;
  return time;
}

inline uint32_t Scene::getfuben_timer() {
  uint32_t timer = static_cast<uint32_t>(ID_INVALID);
  if (fuben_data_) timer = fuben_data_->scenetimer;
  return timer;
}

inline int32_t Scene::getfuben_param(const uint32_t index) {
  int32_t data = -1;
  if (fuben_data_ && (index > 0 && index < SCENE_FUBEN_DATA_PARAM_MAX))
    data = fuben_data_->param[index];
  return data;
}

inline void Scene::fuben_setdata(fuben_data_t *fuben_data) {
  fuben_data_ = fuben_data;
}

inline int32_t Scene::get_rand100() {
  __ENTER_FUNCTION
    int32_t result = -1;
    result = static_cast<int32_t>(randomtable100_.getrand());
    return result;
  __LEAVE_FUNCTION
    return -1;
}

inline int32_t Scene::get_rand10000() {
  __ENTER_FUNCTION
    int32_t result = -1;
    result = static_cast<int32_t>(randomtable10000_.getrand());
    return result;
  __LEAVE_FUNCTION
    return -1;
}

inline char *Scene::get_map_filename() {
  return map_filename_;
}
   
inline void Scene::settype(uint8_t type) {
  type_ = type;
}
   
inline uint8_t Scene::gettype() const {
  return type_;
}
   
inline void Scene::set_safelevel(uint8_t level) {
  safelevel_ = level;
}
   
inline uint8_t Scene::get_safelevel() const {
  return safelevel_;
}
   
inline void Scene::set_restrictivemode(uint8_t mode) {
  restrictivemode_ = mode;
}
   
inline uint8_t Scene::get_restrictivemode() const {
  return restrictivemode_;
}
   
inline void Scene::setpunish(bool punish) {
  punish_ = punish;
}
   
inline bool Scene::getpunish() const {
  return punish_;
}

inline void Scene::timerstart(uint32_t destroytime, uint32_t nowtime) {
  timer_.start(destroytime, nowtime);
}
   
inline void Scene::timerstop() {
  timer_.cleanup();
}

inline int32_t Scene::get_clientres() const {
  return clientres_;
}
   
inline structure::position_t *Scene::get_canenter_position() {
  return &canenter_position_;
}
   
inline void Scene::set_canenter_position(int32_t x, int32_t z) {
  canenter_position_.x = static_cast<float>(x);
  canenter_position_.z = static_cast<float>(z);
}

inline uint32_t Scene::get_figtttime() const {
  return fighttime_;
}
   
inline void Scene::set_fighttime(uint32_t time) {
  fighttime_ = time;
}
   
inline void Scene::setthread(Thread *thread) {
  thread_ = thread;
}
   
inline Thread *Scene::getthread() {
  return thread_;
}
   
inline void Scene::setlock(bool lock) {
  detectlock_ = lock;
}
   
inline void Scene::incmoney(int32_t count) {
  money_ += count;
}
   
inline void Scene::decmoney(int32_t count) {
  money_ -= count;
}
   
inline int32_t Scene::getmoney() const {
  return money_;
}
   
inline void Scene::set_broadcast_radius(int32_t radius) {
  broadcast_radius_ = radius;
}
   
inline int32_t Scene::get_broadcast_radius() const {
  return broadcast_radius_;
}

inline scene::Map *Scene::getmap() {
  return map_;
}

inline scene::manager::Net *Scene::get_netmanager() {
  return netmanager_;
}

inline structure::manager::Object *Scene::get_objectmanager() {
  return objectmanager_;
}

inline structure::manager::ObjectSingle *Scene::get_humanmanager() {
  return humanmanager_;
}

inline structure::manager::ObjectSingle *Scene::get_monstermanager() {
  return monstermanager_;
}

inline scene::manager::TimerDoing *Scene::get_timerdoing_manager() {
  return timerdoing_manager_;
}

inline Timer *Scene::gettimer() {
  return timers_;
}

inline ChatPipeline *Scene::get_chatpipeline() {
  return chatpipeline_;
}

inline gridinfo_t *Scene::get_gridinfo() {
  return &gridinfo_;
}

inline pf_script::lua::Interface *Scene::get_luainterface() {
  return luainterface_;
}

inline scene::manager::RecycleConnection *Scene::get_recycleconnection_manager() {
  return recycle_connectionmanager_;
}

inline scene::manager::DropPosition *Scene::get_dropposition_manager() {
  return dropposition_manager_;
}

inline scene::manager::Region *Scene::get_regionmanager() {
  return regionmanager_;
}

inline scene::manager::Variable *Scene::get_variablemanager() {
  return variable_manager_;
}

inline bool Scene::isinit() const {
  return isinit_;
}

inline sceneid_t Scene::getid() const {
  return id_;
}

inline uint8_t Scene::getstatus() const {
  return status_;
}

inline void Scene::setstatus(uint8_t status) {
  status_ = status;
}

inline scene::manager::Scene *Scene::get_scenemanager() {
  return scenemanager_;
}
   
inline void Scene::set_scenemanager(scene::manager::Scene *scenemanager) {
  scenemanager_ = scenemanager;
}

}; //namespace scene

#endif //SCENE_SCENE_INL_
