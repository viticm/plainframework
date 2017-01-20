#include "pf/base/log.h"
#include "pf/base/time_manager.h"
#include "pf/base/string.h"
#include "structure/math.h"
#include "structure/object.h"
#include "structure/objectlist.h"
#include "structure/manager/object.h"
#include "structure/manager/objectsingle.h"
#include "scene/define.h"
#include "scene/timer.h"
#include "scene/chat_pipeline.h"
#include "scene/map.h"
#include "scene/grid.h"
#include "scene/scaner.h"
#include "scene/thread.h"
#include "scene/manager/net.h"
#include "scene/manager/recycleconnection.h"
#include "scene/manager/variable.h"
#include "scene/manager/timerdoing.h"
#include "scene/manager/region.h"
#include "scene/manager/pkregion.h"
#include "scene/manager/dropposition.h"
#include "scene/manager/scene.h"
#include "scene/scene.h"

namespace scene {

Scene::Scene() {
  __ENTER_FUNCTION
    packet_newplayer_ = NULL;
    packet_newplayer_move_ = NULL;
    packet_newplayer_death_ = NULL;
    packet_newmonster_ = NULL;
    packet_newmonster_move_ = NULL;
    packet_newmonster_death_ = NULL;
    packet_deleteobject_ = NULL;
    threadid_ = static_cast<uint64_t>(ID_INVALID);
    memset(map_filename_, 0, sizeof(map_filename_));
    clientres_ = ID_INVALID;
    fighttime_ = 0;
    tick_ = 0;
    tickend_ = 0;
    detectlock_ = false;
    thread_ = NULL;
    backuptime_ = 0;
    starttime_ = 0;
    currenttime_ = 0;
    last_frametime_ = 0;
    currentframes_ = 0;
    currenttimes_ = 0;
    lastFPS_ = 0;
    money_ = 0;
    is_fristinit_ = false;
    fuben_data_ = NULL;
    luainterface_ = NULL;
    gridsize_ = 10;
    costtime_input_ = 0;
    costtime_output1_ = 0;
    costtime_output2_ = 0;
    costtime_process_ = 0;
    costtime_cache_ = 0;
    costtime_heartbeat_ = 0;
    isinit_ = false;
    id_ = ID_INVALID;
    status_ = kStatusSleep;
    type_ = static_cast<uint8_t>(kTypeInvalid);
    map_ = NULL;
    grid_ = NULL;
    netmanager_ = NULL;
    recycle_connectionmanager_ = NULL;
    objectmanager_ = NULL;
    humanmanager_ = NULL;
    monstermanager_ = NULL;
    regionmanager_ = NULL;
    timerdoing_manager_ = NULL;
    pkregion_manager_ = NULL;
    variable_manager_ = NULL;
    dropposition_manager_ = NULL;
    scenemanager_ = NULL;
    timers_ = NULL;
    chatpipeline_ = NULL;
    safelevel_ = 0;
    restrictivemode_ = 0;
    punish_ = false;
    broadcast_radius_ = 2;
    config_bool_.init(100);
    registerconfig(kConfigMapIsActive, false);
    registerconfig(kConfigNetManagerIsActive, false);
    registerconfig(kConfigObjectManagerIsActive, false);
    registerconfig(kConfigHumanManagerIsActive, false);
    registerconfig(kConfigMonsterManagerIsActive, false);
    registerconfig(kConfigTimerDoingManagerIsActive, false);
    registerconfig(kConfigTimersIsActive, false);
    registerconfig(kConfigChatPipelineIsActive, false);
    registerconfig(kConfigRecycleConnectionManagerIsActive, false);
    registerconfig(kConfigDropPositionManagerIsActive, false);
    registerconfig(kConfigRegionManagerIsActive, false);
    registerconfig(kConfigPKResionManagerIsActive, false);
    registerconfig(kConfigFubenIsActive, false);
    registerconfig(kConfigScriptIsActive, false);
    registerconfig(kConfigScriptMutliModeIsActive, false);
  __LEAVE_FUNCTION
}

Scene::~Scene() {
  __ENTER_FUNCTION
    SAFE_DELETE(fuben_data_);
    SAFE_DELETE_ARRAY(grid_);
  __LEAVE_FUNCTION
}

bool Scene::load(load_t *load) {
  __ENTER_FUNCTION
    if (NULL == scenemanager_) return false; //场景启动应放在场景管理器中
    using namespace pf_base::string;
    char temp[FILENAME_MAX] = {0};
    char map_filename[FILENAME_MAX] = {0};
    char monster_filename[FILENAME_MAX] = {0};
    char region_filename[FILENAME_MAX] ={0};
    char pkregion_filename[FILENAME_MAX] = {0};
    bool is_pkregion_exist = false;
    safecopy(map_filename, load->map_filename, sizeof(map_filename));
    safecopy(monster_filename, load->monster_filename, sizeof(monster_filename));
    safecopy(region_filename, load->region_filename, sizeof(region_filename));
    safecopy(pkregion_filename, 
             load->pkregion_filename, 
             sizeof(pkregion_filename));
    is_pkregion_exist = load->is_pkregion_exist;
    bool result = remove_allobject();
    if (!result) {
      SLOW_ERRORLOG(SCENE_MODULENAME,
                    "[scene] (Scene::load) remove all object error,"
                    " id: %d",
                    getid());
      return false;
    }
    SLOW_LOG(SCENE_MODULENAME, "[scene] (Scene::load) start, id: %d", getid());
    if (getconfig_boolvalue(kConfigMapIsActive)) {
      scenemanager_->get_fullpath(temp, map_filename);
      result = map_->load(temp);
      if (!result) {
        SLOW_ERRORLOG(SCENE_MODULENAME,
                      "[scene] (Scene::load) error of map,"
                      " id: %d",
                      getid());
        return false;
      }
      //grid
      SAFE_DELETE_ARRAY(grid_);
      int32_t x = static_cast<int32_t>(map_->getx() / gridsize_);
      int32_t z = static_cast<int32_t>(map_->getz() / gridsize_);
      if (static_cast<int32_t>(map_->getx()) % gridsize_ > 0) ++x;
      if (static_cast<int32_t>(map_->getz()) % gridsize_ > 0) ++z;
      gridinfo_.size = static_cast<uint16_t>(x * z);
      gridinfo_.width = static_cast<uint16_t>(x);
      gridinfo_.height = static_cast<uint16_t>(z);
      grid_ = new Grid[gridinfo_.size];
      if (NULL == grid_) {
        SLOW_ERRORLOG(SCENE_MODULENAME,
                      "[scene] (Scene::load) error alloc memory for grid,"
                      " id: %d",
                      getid());
        return false;
      }
      for (uint16_t i = 0; i < gridinfo_.size; ++i) getgrid(i)->setid(i);
    }
    scene::setting_t *setting = scenemanager_->get_setting(getid());
    if (setting) {
      if (kTypeLogic == setting->type) {
        is_fristinit_ = false;
      } else if (kTypeFuben == setting->type) {
        fuben_data_ = new fuben_data_t;
        Assert(fuben_data_);
      }
      if (kTypeFuben == setting->type) {
        clientres_ = scenemanager_->get_resid_by_navname(map_filename);
        const scene::setting_t *aliassetting = 
          scenemanager_->getsetting_byclient_resid(clientres_);
        if (aliassetting) {
          settype(aliassetting->type);
          set_safelevel(aliassetting->safelevel);
          set_restrictivemode(aliassetting->restrictivemode);
          set_canenter_position(aliassetting->defaultx, aliassetting->defaultz);
        }
      } else {
        clientres_ = setting->clientres;
        settype(setting->type);
        set_safelevel(setting->safelevel);
        set_restrictivemode(setting->restrictivemode);
        setpunish(setting->punish);
        set_canenter_position(setting->defaultx, setting->defaultz);
      }
    }
    
    //timer doing manager
    if (getconfig_boolvalue(kConfigTimerDoingManagerIsActive)) {
      timerdoing_manager_->init();
      timerdoing_manager_->setscene(this);
      SLOW_LOG(SCENE_MODULENAME, "[scene] (Scene::load) timer manager success");
      if (!timers_->isinit()) {
        timers_->init(scenemanager_->get_settings()->timercount_max, this);
      } else {
        timers_->cleanup();
      }
    }

    //region manager
    if (getconfig_boolvalue(kConfigRegionManagerIsActive)) {
      memset(temp, 0, sizeof(temp));
      scenemanager_->get_fullpath(temp, region_filename);
      regionmanager_->destroy();
      result = regionmanager_->init(temp);
      if (!result) {
        SLOW_ERRORLOG(SCENE_MODULENAME,
                      "[scene] (Scene::load) region manager error,"
                      " id: %d",
                      getid());
        return false;
      }
    }

    //pk region manager
    if (getconfig_boolvalue(kConfigPKResionManagerIsActive)) {
      if (is_pkregion_exist) {
        memset(temp, 0, sizeof(temp));
        scenemanager_->get_fullpath(temp, pkregion_filename);
        pkregion_manager_->destroy();
        result = pkregion_manager_->init(temp, getid());
        if (!result) {
          SLOW_ERRORLOG(SCENE_MODULENAME,
                        "[scene] (Scene::load) pk region manager error,"
                        " id: %d",
                        getid());
          return false;
        }
      }
    }

    //drop position manager
    if (getconfig_boolvalue(kConfigDropPositionManagerIsActive)) {
      dropposition_manager_->cleanup();
      dropposition_manager_->init(map_->getx(), map_->getz());
      SLOW_LOG(SCENE_MODULENAME, 
               "[scene] (Scene::load) drop position manager success");
    }

    SLOW_LOG(SCENE_MODULENAME, "[scene] (Scene::load) finsh, id: %d", getid());
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Scene::init(sceneid_t id) {
  id_ = id;
  status_ = kStatusSleep;
  return true;
}

bool Scene::tick() {
  __ENTER_FUNCTION
    
    ++(performance_.ticks_[kPerformanceTypeTickBefore]);

    //step 1 logic runing
    if (!statuslogic()) {
      status_falselogic();
      return true;
    }
    ++(performance_.ticks_[kPerformanceTypeTickLogic]);

    //step 2 net manager select
    try {
      netmanager_->select();
    } catch (...) {
      SaveErrorLog();
    }
    ++(performance_.ticks_[kPerformanceTypeTickSelect]);

    //step 3 net manager process exception
    try {
      if (getconfig_boolvalue(kConfigNetManagerIsActive))
        netmanager_->processexception();
    } catch (...) {
      SaveErrorLog();
    }
    ++(performance_.ticks_[kPerformanceTypeTickProcessExceptions]);

    //step 4 net manager process input
    uint32_t start_ticktime = TIME_MANAGER_POINTER->get_tickcount();
    uint32_t end_ticktime = 0;
    try {
      if (getconfig_boolvalue(kConfigNetManagerIsActive))
        netmanager_->processinput();
    } catch (...) {
      SaveErrorLog();
    }
    ++(performance_.ticks_[kPerformanceTypeTickProcessInputs]);
    end_ticktime = TIME_MANAGER_POINTER->get_tickcount();
    costtime_input_ = end_ticktime - start_ticktime;
    start_ticktime = end_ticktime;

    //step 5 net manager process command
    try {
      netmanager_->processcommand();
    } catch (...) {
      SaveErrorLog();
    }
    ++(performance_.ticks_[kPerformanceTypeTickProcessCommands]);
    end_ticktime = TIME_MANAGER_POINTER->get_tickcount();
    costtime_process_ = end_ticktime - start_ticktime;
    start_ticktime = end_ticktime;

    //step 6 net manager process cache command
    try {
      if (getconfig_boolvalue(kConfigNetManagerIsActive))
        netmanager_->process_cachecommand();
    } catch (...) {
      SaveErrorLog();
    }
    ++(performance_.ticks_[kPerformanceTypeTickProcessCacheCommands]);
    end_ticktime = TIME_MANAGER_POINTER->get_tickcount();
    costtime_cache_ = end_ticktime - start_ticktime;
    start_ticktime = end_ticktime;

    //step 7 net manager process output
    try {
      if (getconfig_boolvalue(kConfigNetManagerIsActive))
        netmanager_->processoutput();
    } catch (...) {
      SaveErrorLog();
    }
    ++(performance_.ticks_[kPerformanceTypeTickProcessOutputsBHB]);
    end_ticktime = TIME_MANAGER_POINTER->get_tickcount();
    costtime_output1_ = end_ticktime - start_ticktime;
    start_ticktime = end_ticktime;

    //step 8 heart beat
    try {
      heartbeat();
    } catch (...) {
      SaveErrorLog();
    }
    ++(performance_.ticks_[kPerformanceTypeTickHeartBeat]);
    end_ticktime = TIME_MANAGER_POINTER->get_tickcount();
    costtime_heartbeat_ = end_ticktime - start_ticktime;
    start_ticktime = end_ticktime;

    //step 9 net manager process output again
    try {
      if (getconfig_boolvalue(kConfigNetManagerIsActive))
        netmanager_->processoutput();
    } catch (...) {
      SaveErrorLog();
    }
    ++(performance_.ticks_[kPerformanceTypeTickProcessOutputsAHB]);
    end_ticktime = TIME_MANAGER_POINTER->get_tickcount();
    costtime_output2_ = end_ticktime - start_ticktime;
    start_ticktime = end_ticktime;

    //else logic
    uint32_t current_ticktime = TIME_MANAGER_POINTER->get_tickcount();
    if (0 == last_frametime_) last_frametime_ = current_ticktime;
    uint32_t logic_ticktime = current_ticktime - last_frametime_;
    last_frametime_ = current_ticktime;
    ++currentframes_;
    currenttimes_ += logic_ticktime;
    if (currentframes_ > 10) {
      lastFPS_ = currenttimes_;
      currentframes_ = 0;
      currenttimes_ = 0;
    }
    ++tickend_;
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Scene::statuslogic() {
  __ENTER_FUNCTION
    bool result = false;
    switch (status_) {
      case kStatusSleep:
        break;
      case kStatusSelect:
        break;
      case kStatusLoading:
        break;
      case kStatusInit: {
        setstatus(kStatusRuning);
        if (!oninit() && kTypeFuben == gettype()) {
          setstatus(kStatusSleep);
        }
        break;
      }
      case kStatusRuning:
        result = true;
        break;
      default:
        break;
    }
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Scene::status_falselogic() {
  __ENTER_FUNCTION
    bool result = false;
    try {
      uint32_t time = TIME_MANAGER_POINTER->get_tickcount();
      result = recycle_connectionmanager_->heartbeat(time);
    } catch (...) {
      SaveErrorLog();
    }
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Scene::heartbeat() {
  __ENTER_FUNCTION
    bool result = false;
    uint32_t now_ticktime = TIME_MANAGER_POINTER->get_tickcount();
    starttime_ = TIME_MANAGER_POINTER->get_start_time();
    currenttime_ = TIME_MANAGER_POINTER->get_saved_time();
    backuptime_ = now_ticktime;
    TIME_MANAGER_POINTER->reset_time();
    
    //step 1 check close
    if (quittimer_.isstart()) {
      if (quittimer_.counting(now_ticktime)) {
        SLOW_LOG(SCENE_MODULENAME,
                 "[scene] (Scene::heartbeat) %d colsing",
                 getid());
        setstatus(kStatusSleep);
        quittimer_.cleanup();
        if (getconfig_boolvalue(kConfigFubenIsActive)) fuben_data_->clear();
        fuben_quittimer_.cleanup();
        if (getconfig_boolvalue(kConfigNetManagerIsActive))
          netmanager_->destroy();
        onclose();
        return true;
      }
    } else {
      if (getconfig_boolvalue(kConfigNetManagerIsActive) &&
          netmanager_->getcount() > 0) {
        fuben_quittimer_.cleanup();
      } else if (kTypeFuben == gettype()) {
        if (!fuben_quittimer_.isstart()) {
          fuben_quittimer_.start(getfuben_nouser_closetime(), now_ticktime);
          SLOW_LOG(SCENE_MODULENAME,
                   "[scene] %d will close after %d seconds!",
                   getid(),
                   getfuben_nouser_closetime() / 1000);
        }
        if (fuben_quittimer_.counting(now_ticktime)) close();
      }
    }
    if (!is_fristinit_) {
      is_fristinit_ = true;
      oninit();
    }
    ++(performance_.ticks_[kPerformanceTypeTickHeartBeatCloseLogic]);
    
    //step 2 timer
    try {
      if (timer_.counting(now_ticktime)) ontimer(now_ticktime);
    } catch (...) {
      SaveErrorLog();
    }
    ++(performance_.ticks_[kPerformanceTypeTickHeartBeatTimer]);

    //step 3 net manager
    try {
      if (getconfig_boolvalue(kConfigNetManagerIsActive)) {
        result = netmanager_->heartbeat(now_ticktime);
        Assert(result);
      }
    } catch (...) {
      SaveErrorLog();
    }
    ++(performance_.ticks_[kPerformanceTypeTickHeartBeatNetManager]);

    //step 4 human manager
    try {
      if (getconfig_boolvalue(kConfigHumanManagerIsActive)) {
        result = humanmanager_->heartbeat(now_ticktime);
        Assert(result);
      }
    } catch (...) {
      SaveErrorLog();
    }
    ++(performance_.ticks_[kPerformanceTypeTickHeartBeatHumanManager]);

    //step 5 monster manager
    try {
      if (getconfig_boolvalue(kConfigMonsterManagerIsActive)) {
        result = monstermanager_->heartbeat(now_ticktime);
        Assert(result);
      }
    } catch (...) {
      SaveErrorLog();
    }
    ++(performance_.ticks_[kPerformanceTypeTickHeartBeatMonsterManager]);

    //step 6 object manager
    try {
      if (getconfig_boolvalue(kConfigObjectManagerIsActive)) {
        //result = objectmanager_->heartbeat(now_ticktime);
        //Assert(result);
      }
    } catch (...) {
      SaveErrorLog();
    }
    ++(performance_.ticks_[kPerformanceTypeTickHeartBeatObjectManager]);

    //step 7 chat pipeline
    try {
      if (getconfig_boolvalue(kConfigChatPipelineIsActive)) {
        result = chatpipeline_->heartbeat(now_ticktime);
        Assert(result);
      }
    } catch (...) {
      SaveErrorLog();
    }
    ++(performance_.ticks_[kPerformanceTypeTickHeartBeatChatPipeline]);

    //step 8 timers
    try {
      if (getconfig_boolvalue(kConfigTimersIsActive)) {
        if (timers_->refeshtimer_.counting(now_ticktime)) {
          timers_->onevent(now_ticktime);
        }
      }
    } catch (...) {
      SaveErrorLog();
    }
    ++(performance_.ticks_[kPerformanceTypeTickHeartBeatTimers]);

    //step 9 variable manager
    try {
      if (getconfig_boolvalue(kConfigVariableManagerIsActive)) {
        result = variable_manager_->heartbeat(now_ticktime);
        Assert(result);
      }
    } catch (...) {
      SaveErrorLog();
    }
    ++(performance_.ticks_[kPerformanceTypeTickHeartBeatVariableManager]);

    return true;
  __LEAVE_FUNCTION
    return false;
}

void Scene::registerconfig(int32_t key, bool value) {
  __ENTER_FUNCTION
    if (config_bool_.isfind(key)) {
      SLOW_WARNINGLOG(SCENE_MODULENAME,
                      "[scene] (Scene::registerconfig) repeat the key: %d",
                      key);
      return;
    }
    config_bool_.add(key, value);
  __LEAVE_FUNCTION
}

bool Scene::setconfig(int32_t key, bool value) {
  __ENTER_FUNCTION
    bool result = config_bool_.set(key, value);
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Scene::getconfig_boolvalue(int32_t key) {
  __ENTER_FUNCTION
    bool result = config_bool_.get(key);
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Scene::process_cachecommand() {
  __ENTER_FUNCTION
    bool result = true;
    if (getconfig_boolvalue(kConfigNetManagerIsActive)) {
      result = netmanager_->process_cachecommand();
    }
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Scene::packet_send(pf_net::packet::Base *packet,
                        int16_t connectionid,
                        uint32_t flag) {
  __ENTER_FUNCTION
    bool result = true;
    if (getconfig_boolvalue(kConfigNetManagerIsActive)) {
      result = netmanager_->sendpacket(packet, connectionid, flag);
    }
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Scene::oninit() {
  __ENTER_FUNCTION
    if (kTypeFuben == gettype()) {
      uint32_t now_ticktime = TIME_MANAGER_POINTER->get_tickcount();
      if (getconfig_boolvalue(kConfigFubenIsActive)) {
        timerstart(fuben_data_->scenetimer, now_ticktime);
      }
    }
    if (getconfig_boolvalue(kConfigScriptIsActive)) {
      get_luainterface()->run_scriptfunction(
          SCENE_SCRIPTID,
          "on_scene_init",
          static_cast<int64_t>(getid()));
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

void Scene::ontimer(uint32_t time) {
  __ENTER_FUNCTION
    if (getconfig_boolvalue(kConfigScriptIsActive)) {
      get_luainterface()->run_scriptfunction(
          SCENE_SCRIPTID,
          "on_scene_timer",
          static_cast<int64_t>(getid()),
          static_cast<int64_t>(time));
    }
  __LEAVE_FUNCTION
}

void Scene::onquit() {
  __ENTER_FUNCTION
    if (getconfig_boolvalue(kConfigScriptIsActive)) {
      get_luainterface()->run_scriptfunction(
          SCENE_SCRIPTID,
          "on_scene_quit",
          static_cast<int64_t>(getid()));
    }
    remove_allobject();
  __LEAVE_FUNCTION
}

void Scene::onclose() {
  __ENTER_FUNCTION
    if (getconfig_boolvalue(kConfigMapIsActive)) map_->cleanup();
  __LEAVE_FUNCTION
}

void Scene::onenter(pf_net::connection::Base *connection) {
  USE_PARAM(connection);
  //do nothing
}

void Scene::ondisconnect(pf_net::connection::Base *connection) {
  USE_PARAM(connection);
  //do nothing
}

void Scene::onleave(pf_net::connection::Base *connection) {
  USE_PARAM(connection);
  //do nothing
}

void Scene::onnotify(sceneid_t sceneid) {
  __ENTER_FUNCTION
    if (getconfig_boolvalue(kConfigScriptIsActive)) {
      get_luainterface()->run_scriptfunction(
          SCENE_SCRIPTID,
          "on_scene_notify",
          static_cast<int64_t>(getid()),
          static_cast<int64_t>(sceneid));
    }
  __LEAVE_FUNCTION
}

bool Scene::close() {
  __ENTER_FUNCTION
    if (!quittimer_.isstart() &&
        getstatus() != kStatusSleep &&
        gettype() != kTypeLogic) {
      uint32_t time = 5000;
      quittimer_.start(time, TIME_MANAGER_POINTER->get_tickcount());
      onquit();
      FAST_DEBUGLOG(kDebugLogFile,
                    "[scene] (Scene::close) %d will close after %d seconds",
                    getid(),
                    time / 1000);
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Scene::scan(Scaner *scaner) {
  __ENTER_FUNCTION
    if (NULL == scaner) return false;
    if (!scaner->on_beforescan()) return true;
    structure::vrect_t vrect;
    get_vrect_inradius(&vrect, scaner->gridradius_, scaner->gridid_);
    bool needbreak = false;
    for (int32_t z = vrect.startz; z < vrect.endz; ++z) {
      if (needbreak) break;
      for (int32_t x = vrect.startx; x < vrect.endx; ++x) {
        if (needbreak) break;
        gridid_t gridid = x + z * gridinfo_.width;
        if (!scaner->on_needscan(gridid)) continue;
        structure::ObjectList *list = NULL;
        scene::Grid *grid = getgrid(gridid);
        Assert(grid);
        if (scaner->scanhuman_) {
          list = grid->get_humanlist();
        } else {
          list = grid->get_objectlist();
        }
        Assert(list);
        structure::objectlist_node_t *pointer = list->head_.next;
        while (pointer && pointer != &list->tail_) {
          structure::Object *object = NULL;
          object = pointer->node;
          uint8_t result = static_cast<uint8_t>(scaner->on_findobject(object));
          if (kScanerTurnContinue == result) continue;
          if (kScanerTurnBreak == result) break;
          if (kScanerTurnReturn == result) {
            needbreak = true;
            break;
          }
          pointer = pointer->next; //move to next pointer
        } //while
      } //for 
    } //for 
  __LEAVE_FUNCTION
    return false;
}

bool Scene::scanhuman(structure::position_t *position,
                      float range,
                      structure::objectlist_t *list) {
  __ENTER_FUNCTION
    gridid_t gridid = calc_gridid(position);
    structure::vrect_t vrect;
    get_vrect_inradius(&vrect, static_cast<int32_t>(range), gridid);
    list->count = 0;
    for (int32_t z = vrect.startz; z < vrect.endz; ++z) {
      for (int32_t x = vrect.startx; x < vrect.endx; ++x) {
        gridid_t _gridid = x + z * gridinfo_.width;
        structure::ObjectList *objectlist = getgrid(_gridid)->get_humanlist();
        structure::objectlist_node_t *pointer = objectlist->head_.next;
        while (pointer && pointer != &objectlist->tail_) {
          structure::Object *object = pointer->node;
          if (NULL == object) {
            Assert(false);
            return false;
          }
          float distance = structure::math::sqrtex(position, object->getposition());
          if (distance > range) continue;
          list->list[list->count++] = object; //find it
          if (list->count >= STRUCTURE_OBJECTLIST_MAX) return true;
        }
      } //for
    } //for
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Scene::broadcast(pf_net::packet::Base *packet,
                      structure::Object *object,
                      bool sendme,
                      bool lockme_only) {
  __ENTER_FUNCTION
    if (NULL == object) return false;
    gridid_t gridid = object->get_gridid();
    if (ID_INVALID == gridid) return false;
    structure::vrect_t vrect;
    get_vrect_inradius(&vrect, get_broadcast_radius(), gridid);
    for (int32_t z = vrect.startz; z < vrect.endz; ++z) {
      for (int32_t x = vrect.startx; x < vrect.endx; ++x) {
        gridid_t _gridid = x + z * gridinfo_.width;
        structure::ObjectList *objectlist = getgrid(_gridid)->get_humanlist();
        structure::objectlist_node_t *pointer = objectlist->head_.next;
        while (pointer && pointer != &objectlist->tail_) {
          structure::Object *_object = pointer->node;
          if (NULL == _object) {
            Assert(false);
            return false;
          }
          //是否发送给自己
          if (!sendme && object->getid() == _object->getid()) continue;
          //是否只发送给关注我的对象
          if (lockme_only && object->getid() != _object->getid()) {
            if (_object->get_lockedtarget() != object->getid()) continue;
          }
          _object->get_netconnection()->sendpacket(packet);
        }
      } //for
    } //for
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Scene::broadcast_scene(pf_net::packet::Base *packet) {
  __ENTER_FUNCTION
    netmanager_->broadcast(packet);
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Scene::broadcast(pf_net::packet::Base *packet, gridid_t gridid) {
  __ENTER_FUNCTION
    if (ID_INVALID == gridid) return false;
    structure::vrect_t vrect;
    get_vrect_inradius(&vrect, get_broadcast_radius(), gridid);
    for (int32_t z = vrect.startz; z < vrect.endz; ++z) {
      for (int32_t x = vrect.startx; x < vrect.endx; ++x) {
        gridid_t _gridid = x + z * gridinfo_.width;
        structure::ObjectList *objectlist = getgrid(_gridid)->get_humanlist();
        structure::objectlist_node_t *pointer = objectlist->head_.next;
        while (pointer && pointer != &objectlist->tail_) {
          structure::Object *object = pointer->node;
          if (NULL == object) {
            Assert(false);
            return false;
          }
          object->get_netconnection()->sendpacket(packet);
        }
      } //for
    } //for
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Scene::broadcast_chat(pf_net::packet::Base *packet, gridid_t gridid) {
  __ENTER_FUNCTION
    if (ID_INVALID == gridid) {
      netmanager_->broadcast(packet, chatpipeline_);
    } else {
      structure::vrect_t vrect;
      get_vrect_inradius(&vrect, get_broadcast_radius(), gridid);
      for (int32_t z = vrect.startz; z < vrect.endz; ++z) {
        for (int32_t x = vrect.startx; x < vrect.endx; ++x) {
          gridid_t _gridid = x + z * gridinfo_.width;
          structure::ObjectList *objectlist = getgrid(_gridid)->get_humanlist();
          structure::objectlist_node_t *pointer = objectlist->head_.next;
          while (pointer && pointer != &objectlist->tail_) {
            structure::Object *object = pointer->node;
            if (NULL == object) {
              Assert(false);
              return false;
            }
            object->get_netconnection()->sendpacket(packet);
            chatpipeline_->dec_validcount();
          }
        } //for
      } //for
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Scene::scanhuman(gridid_t gridid,
                      uint32_t radius_gridcount,
                      structure::objectlist_t *list) {
  __ENTER_FUNCTION
    list->count = 0;
    structure::vrect_t vrect;
    get_vrect_inradius(&vrect, radius_gridcount, gridid);
    for (int32_t z = vrect.startz; z < vrect.endz; ++z) {
      for (int32_t x = vrect.startx; x < vrect.endx; ++x) {
        gridid_t _gridid = x + z * gridinfo_.width;
        structure::ObjectList *objectlist = getgrid(_gridid)->get_humanlist();
        structure::objectlist_node_t *pointer = objectlist->head_.next;
        while (pointer && pointer != &objectlist->tail_) {
          structure::Object *object = pointer->node;
          if (NULL == object) {
            Assert(false);
            return false;
          }
          list->list[list->count++] = object; //find it
          if (list->count >= STRUCTURE_OBJECTLIST_MAX) return true;
        }
      } //for
    } //for
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Scene::scanhuman(gridid_t gridid_A,
                      gridid_t gridid_B,
                      uint32_t gridradius,
                      structure::objectlist_t *list) {
  __ENTER_FUNCTION
    list->count = 0;
    structure::vrect_t vrect_A;
    structure::vrect_t vrect_B;
    get_vrect_inradius(&vrect_A, gridradius, gridid_A);
    get_vrect_inradius(&vrect_B, gridradius, gridid_B);
    for (int32_t z = vrect_A.startz; z < vrect_A.endz; ++z) {
      for (int32_t x = vrect_B.startx; x < vrect_B.endx; ++x) {
        if (!vrect_B.iscontinue(x, z)) {
          gridid_t _gridid = x + z * gridinfo_.width;
          structure::ObjectList *objectlist = getgrid(_gridid)->get_humanlist();
          structure::objectlist_node_t *pointer = objectlist->head_.next;
          while (pointer && pointer != &objectlist->tail_) {
            structure::Object *object = pointer->node;
            if (NULL == object) {
              Assert(false);
              return false;
            }
            list->list[list->count++] = object; //find it
            if (list->count >= STRUCTURE_OBJECTLIST_MAX) return true;
          } //while
        }
      } //for
    } //for
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Scene::scanobject(gridid_t gridid,
                       uint32_t radius_gridcount,
                       structure::objectlist_t *list) {
  __ENTER_FUNCTION
    list->count = 0;
    structure::vrect_t vrect;
    get_vrect_inradius(&vrect, radius_gridcount, gridid);
    for (int32_t z = vrect.startz; z < vrect.endz; ++z) {
      for (int32_t x = vrect.startx; x < vrect.endx; ++x) {
        gridid_t _gridid = x + z * gridinfo_.width;
        structure::ObjectList *objectlist = getgrid(_gridid)->get_objectlist();
        structure::objectlist_node_t *pointer = objectlist->head_.next;
        while (pointer && pointer != &objectlist->tail_) {
          structure::Object *object = pointer->node;
          if (NULL == object) {
            Assert(false);
            return false;
          }
          list->list[list->count++] = object; //find it
          if (list->count >= STRUCTURE_OBJECTLIST_MAX) return true;
        }
      } //for
    } //for
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Scene::scanobject(gridid_t gridid_A,
                       gridid_t gridid_B,
                       uint32_t gridradius,
                       structure::objectlist_t *list) {
  __ENTER_FUNCTION
    list->count = 0;
    structure::vrect_t vrect_A;
    structure::vrect_t vrect_B;
    get_vrect_inradius(&vrect_A, gridradius, gridid_A);
    get_vrect_inradius(&vrect_B, gridradius, gridid_B);
    for (int32_t z = vrect_A.startz; z < vrect_A.endz; ++z) {
      for (int32_t x = vrect_B.startx; x < vrect_B.endx; ++x) {
        if (!vrect_B.iscontinue(x, z)) {
          gridid_t _gridid = x + z * gridinfo_.width;
          structure::ObjectList *objectlist = getgrid(_gridid)->get_objectlist();
          structure::objectlist_node_t *pointer = objectlist->head_.next;
          while (pointer && pointer != &objectlist->tail_) {
            structure::Object *object = pointer->node;
            if (NULL == object) {
              Assert(false);
              return false;
            }
            list->list[list->count++] = object; //find it
            if (list->count >= STRUCTURE_OBJECTLIST_MAX) return true;
          } //while
        }
      } //for
    } //for
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Scene::sendpacket(pf_net::packet::Base *packet, 
                       structure::objectlist_t *list) {
  __ENTER_FUNCTION
    if (NULL == list) return false;
    for (uint16_t i = 0; i < list->count; ++i) {
      structure::Object *object = list->list[i];
      if (object) object->get_netconnection()->sendpacket(packet);
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Scene::sendpacket_createattr(structure::Object *object,
                                  structure::objectlist_t *list) {
  USE_PARAM(object);
  USE_PARAM(list);
  return true;
}

bool Scene::objectenter(structure::Object *object) {
  __ENTER_FUNCTION
    using namespace structure;
    if (NULL == object) return false;
    int8_t idtype = kObjectIdTypeInvalid;
    if (kObjectTypeHuman == object->gettype()) {
      idtype = kObjectIdTypeHuman;
    } else {
      idtype = kObjectIdTypeNormal;
    }
    bool result = false;
    result = objectmanager_->add(object, idtype);
    if (!result) {
      SLOW_WARNINGLOG(SCENE_MODULENAME,
                      "[scene] (Scene::objectenter) add object"
                      " to manager failed");
      return false;
    }
    int8_t type = object->gettype();
    switch (type) {
      case kObjectTypeHuman: {
        result = humanmanager_->add(object);
        FAST_DEBUGLOG(kDebugLogFile,
                      "[scene] (Scene::objectenter) human"
                      " scene id: %d, guid: %"PRIu64", object id: %d",
                      getid(),
                      object->getguid(),
                      object->getid());
        break;
      }
      case kObjectTypeMonster:
        result = monstermanager_->add(object);
        break;
      default:
        result = false;
        break;
    }
    if (!result) {
      objectmanager_->remove(object->getid());
      SLOW_ERRORLOG(SCENE_MODULENAME,
                    "[scene] (Scene::objectenter) add to single manager failed");
      return false;
    }
    object->setscene(this);
    object->on_enterscene();
    return true;
  __LEAVE_FUNCTION
    return false;
}

void Scene::objectleave(structure::Object *object, 
                        pf_net::packet::Base *packet) {
  __ENTER_FUNCTION
    using namespace structure;
    if (NULL == object) return;
    object->on_leavescene();
    object->setscene(NULL);
    int8_t type = object->gettype();
    switch (type) {
      case kObjectTypeHuman: {
        if (packet) {
          gridid_t gridid = calc_gridid(object->getposition());
          broadcast(packet, gridid);
        }
        humanmanager_->remove(object);
        FAST_DEBUGLOG(kDebugLogFile,
                      "[scene] (Scene::objectleave) human"
                      " scene id: %d, guid: %"PRIu64", object id: %d",
                      getid(),
                      object->getguid(),
                      object->getid());
        break;
      }
      case kObjectTypeMonster: {
        monstermanager_->remove(object);
        break;
      }
      default:
        break;
    }
    objectmanager_->remove(object->getid());
  __LEAVE_FUNCTION
}

structure::Object * Scene::objectnew(int8_t type, int32_t viewgroup) {
  USE_PARAM(type);
  USE_PARAM(viewgroup);
  return NULL;
}

void Scene::objectdelete(structure::Object *object) {
  __ENTER_FUNCTION
    if (NULL == object) {
      FAST_ERRORLOG(kErrorLogFile,
                    "[scene] (Scene::objectdelete) delete a NULL pointer");
      return;
    }
    objectleave(object);
  __LEAVE_FUNCTION
}

bool Scene::regionregister(const region_t *region) {
  __ENTER_FUNCTION
    Assert(region);
    float x, z, xmax, zmax;
    x = region->rect.left;
    z = region->rect.top;
    xmax = region->rect.right;
    zmax = region->rect.bottom;
    Assert(map_->getx() >= xmax && "region out range");
    Assert(map_->getx() >= zmax && "region out range");
    uint32_t _x = static_cast<uint32_t>(x / gridsize_);
    uint32_t _z = static_cast<uint32_t>(z / gridsize_);
    float xpos = static_cast<float>(_x * gridsize_);
    float zpos = static_cast<float>(_z * gridsize_);
    uint16_t gridwidth = get_gridinfo()->width;
    gridid_t gridid = ID_INVALID;
    for (int32_t i = 0; zpos < zmax; zpos += gridsize_, ++i) {
      for (int32_t j = 0; xpos < xmax; xpos += gridsize_, ++j) {
        gridid = static_cast<gridid_t>((_x + j) + (_z + i) *gridwidth);
        getgrid(gridid)->addregion(region);
      }
      xpos = static_cast<float>(_x * gridsize_);
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

const region_t *Scene::regioncheck(structure::Object *object) {
  __ENTER_FUNCTION
    Assert(object);
    const region_t * result = NULL;
    Grid *grid = getgrid(object->get_gridid());
    Assert(grid);
    if (grid->get_regioncount() < 1) return result;
    result = grid->get_currentregion(object->getposition());
    return result;
  __LEAVE_FUNCTION
    return NULL;
}

gridid_t Scene::calc_gridid(const structure::position_t *position) {
  __ENTER_FUNCTION
    gridid_t gridid = ID_INVALID;
    if (NULL == position) {
      SLOW_ERRORLOG(SCENE_MODULENAME,
                    "[scene] (Scene::calc_gridid) NULL == position",
                    " scene id: %d",
                    getid());
      return gridid;
    }
    if (position->x < 0.f || position->z < 0.f) {
      SLOW_ERRORLOG(SCENE_MODULENAME,
                    "[scene] (Scene::calc_gridid) position invalid"
                    " scene id: %d, position->x: %d, position->x: %d",
                    getid(),
                    position->x,
                    position->z);
      return gridid;
    }
    uint32_t x = static_cast<uint32_t>(position->x / gridsize_);
    uint32_t z = static_cast<uint32_t>(position->x / gridsize_);
    gridid = static_cast<gridid_t>(x + z * get_gridinfo()->width);
    return gridid;
  __LEAVE_FUNCTION
    return ID_INVALID;
}

bool Scene::objectgrid_register(structure::Object *object, 
                                gridid_t gridid,
                                structure::Object *other) {
  __ENTER_FUNCTION
    if (NULL == object) return false;
    using namespace structure;
    if (gridid < 0 || gridid >= gridinfo_.size) {
      if (kObjectTypeMonster == object->gettype()) {
        FAST_ERRORLOG(kErrorLogFile,
                      "[scene] (Scene::objectgrid_register) gridid invalid"
                      " scene id: %d, object id: %d, object type: %d"
                      " position(%f, %f)",
                      getid(),
                      object->getid(),
                      object->gettype(),
                      object->getposition()->x,
                      object->getposition()->z);
      }
      Assert(false);
      return false;
    }
    pf_net::packet::Base *packet = object->create_newobject_packet();
    if (packet) {
      try {
        object->process_newobject_packet(packet, other);
        objectlist_t humanlist;
        scanhuman(gridid, get_broadcast_radius(), &humanlist);
        uint8_t i, count;
        count = 0;
        //find all can view object
        for (i = 0; i < humanlist.count; ++i) {
          if (object->can_viewme(humanlist.list[i])) {
            humanlist.list[count++] = humanlist.list[i];
          }
        }
        humanlist.count = count;
        if (humanlist.count > 0) sendpacket(packet, &humanlist);
        object->destroy_newobject_packet(packet);
      } catch (...) {
        object->destroy_newobject_packet(packet);
        throw;
      }
    } else {
      FAST_DEBUGLOG(kErrorLogFile,
                    "[scene] (Scene::objectgrid_register) create packet NULL");
      return false;
    }
    
    //玩家的特殊处理
    if (kObjectTypeHuman == object->gettype()) {
      objectlist_t objectlist;
      scanobject(gridid, get_broadcast_radius(), &objectlist);
      Object *findobject = NULL;
      uint8_t i;
      for (i = 0; i < objectlist.count; ++i) {
        findobject = objectlist.list[i];
        if (findobject->can_viewme(object)) {
          pf_net::packet::Base *packet = findobject->create_newobject_packet();
          if (packet) {
            try {
              findobject->get_netconnection()->sendpacket(packet);
              findobject->destroy_newobject_packet(packet);
            } catch (...) {
              findobject->destroy_newobject_packet(packet);
              throw;
            }
          }
        }
      } //for
    }
    getgrid(gridid)->on_objectenter(object);
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Scene::objectgrid_unregister(structure::Object *object, gridid_t gridid) {
  __ENTER_FUNCTION
    if (NULL == object) return false;
    using namespace structure;
    if (gridid < 0 || gridid >= gridinfo_.size) {
      if (kObjectTypeMonster == object->gettype()) {
        FAST_ERRORLOG(kErrorLogFile,
                      "[scene] (Scene::objectgrid_unregister) gridid invalid"
                      " scene id: %d, object id: %d, object type: %d"
                      " position(%f, %f)",
                      getid(),
                      object->getid(),
                      object->gettype(),
                      object->getposition()->x,
                      object->getposition()->z);
      }
      Assert(false);
      return false;
    }
    if (!getgrid(gridid)->on_objectleave(object)) {
      FAST_ERRORLOG(kErrorLogFile,
                    "[scene] (Scene::objectgrid_unregister) object leave error"
                    " scene id: %d, grid id: %d",
                    getid(),
                    gridid);
    }
    pf_net::packet::Base *packet = object->create_deleteobject_packet();
    if (packet) {
      try {
        objectlist_t humanlist;
        scanhuman(gridid, get_broadcast_radius(), &humanlist);
        uint8_t i, count;
        count = 0;
        //find all can view object
        for (i = 0; i < humanlist.count; ++i) {
          if (object->can_viewme(humanlist.list[i])) {
            humanlist.list[count++] = humanlist.list[i];
          }
        }
        humanlist.count = count;
        if (humanlist.count > 0) sendpacket(packet, &humanlist);
        object->destroy_deleteobject_packet(packet);
      } catch (...) {
        object->destroy_deleteobject_packet(packet);
        throw;
      }
    } else {
      FAST_DEBUGLOG(kErrorLogFile,
                    "[scene] (Scene::objectgrid_unregister) create packet NULL");
      return false;
    }
    
    //玩家的特殊处理
    if (kObjectTypeHuman == object->gettype()) {
      objectlist_t objectlist;
      scanobject(gridid, get_broadcast_radius(), &objectlist);
      Object *findobject = NULL;
      uint8_t i;
      for (i = 0; i < objectlist.count; ++i) {
        findobject = objectlist.list[i];
        if (findobject->can_viewme(object)) {
          pf_net::packet::Base *packet = findobject->create_newobject_packet();
          if (packet) {
            try {
              object->get_netconnection()->sendpacket(packet);
              findobject->destroy_deleteobject_packet(packet);
            } catch (...) {
              findobject->destroy_deleteobject_packet(packet);
              throw;
            }
          }
        }
      } //for
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Scene::objectgrid_change(structure::Object *object,
                              gridid_t new_gridid,
                              gridid_t old_gridid) {
  __ENTER_FUNCTION
    if (NULL == object) return false;
    using namespace structure;
    //leave logic
    if (old_gridid < 0 || old_gridid >= gridinfo_.size) {
      if (kObjectTypeMonster == object->gettype()) {
        FAST_ERRORLOG(kErrorLogFile,
                      "[scene] (Scene::objectgrid_change) old gridid invalid"
                      " scene id: %d, object id: %d, object type: %d,"
                      " position(%f, %f)",
                      getid(),
                      object->getid(),
                      object->gettype(),
                      object->getposition()->x,
                      object->getposition()->z);
      }
      Assert(false);
      return false;
    }
    if (!getgrid(old_gridid)->on_objectleave(object)) {
      FAST_ERRORLOG(kErrorLogFile,
                    "[scene] (Scene::objectgrid_change) object leave error"
                    " scene id: %d, grid id: %d",
                    getid(),
                    old_gridid);
    }
    pf_net::packet::Base *packet = NULL;
    packet = object->create_deleteobject_packet();
    if (packet) {
      objectlist_t humanlist;
      if (new_gridid != ID_INVALID) {
        scanhuman(old_gridid, new_gridid, get_broadcast_radius(), &humanlist);
      } else {
        scanhuman(old_gridid, get_broadcast_radius(), &humanlist);
      }
      uint8_t count, i;
      count = 0;
      for (i = 0; i < humanlist.count; ++i) {
        if (object->can_viewme(humanlist.list[i]))
          humanlist.list[count++] = humanlist.list[i];
      }
      humanlist.count = count;
      if (humanlist.count > 0) sendpacket(packet, &humanlist);
      object->destroy_deleteobject_packet(packet);
    } else {
      FAST_ERRORLOG(kErrorLogFile,
                    "[scene] (Scene::objectgrid_change) 1 create packet NULL");
      return false;
    }
    if (kObjectTypeHuman == object->gettype()) {
      objectlist_t objectlist;
      if (new_gridid != ID_INVALID) {
        scanobject(old_gridid, new_gridid, get_broadcast_radius(), &objectlist);
      } else {
        scanobject(old_gridid, get_broadcast_radius(), &objectlist);
      }
      Object *findobject = NULL;
      for (uint8_t i = 0; i < objectlist.count; ++i) {
        findobject = objectlist.list[i];
        if (NULL == findobject) continue;
        if (findobject->can_viewme(object)) {
          pf_net::packet::Base *packet = 
            findobject->create_deleteobject_packet();
          if (packet) {
            object->get_netconnection()->sendpacket(packet);
            findobject->destroy_deleteobject_packet(packet);
          } else {
            FAST_ERRORLOG(kErrorLogFile,
                          "[scene] (Scene::objectgrid_change)"
                          " 2 create packet NULL");
            return false;
          }
        }
      } //for
    }

    //create logic
    if (new_gridid < 0 || new_gridid >= gridinfo_.size) {
      if (kObjectTypeMonster == object->gettype()) {
        FAST_ERRORLOG(kErrorLogFile,
          "[scene] (Scene::objectgrid_change) new gridid invalid"
          " scene id: %d, object id: %d, object type: %d,"
          " position(%f, %f)",
          getid(),
          object->getid(),
          object->gettype(),
          object->getposition()->x,
          object->getposition()->z);
      }
      Assert(false);
      return false;
    }
    packet = object->create_newobject_packet();
    if (packet) {
      objectlist_t humanlist;
      if (new_gridid != ID_INVALID) {
        scanhuman(new_gridid, old_gridid, get_broadcast_radius(), &humanlist);
      } else {
        scanhuman(new_gridid, get_broadcast_radius(), &humanlist);
      }
      uint8_t count, i;
      count = 0;
      for (i = 0; i < humanlist.count; ++i) {
        if (object->can_viewme(humanlist.list[i])) 
          humanlist.list[count++] = humanlist.list[i];
      }
      humanlist.count = count;
      if (humanlist.count > 0) {
        sendpacket(packet, &humanlist);
        object->process_talk(); //闲聊处理
      }
      object->destroy_newobject_packet(packet);
    } else {
      FAST_ERRORLOG(kErrorLogFile,
                    "[scene] (Scene::objectgrid_change)"
                    " 3 create packet NULL");
      return false;
    }
    if (kObjectTypeHuman == object->gettype()) {
      objectlist_t objectlist;
      if (old_gridid != ID_INVALID) {
        scanobject(new_gridid, old_gridid, get_broadcast_radius(), &objectlist);
      } else {
        scanobject(new_gridid, get_broadcast_radius(), &objectlist);
      }
      Object *findobject = NULL;
      for (uint8_t i = 0; i < objectlist.count; ++i) {
        findobject = objectlist.list[i];
        if (NULL == findobject) continue;
        if (findobject->can_viewme(object)) {
          pf_net::packet::Base *packet = 
            findobject->create_newobject_packet();
          if (packet) {
            object->get_netconnection()->sendpacket(packet);
            findobject->destroy_newobject_packet(packet);
          } else {
            FAST_ERRORLOG(kErrorLogFile,
                          "[scene] (Scene::objectgrid_change)"
                          " 4 create packet NULL");
            return false;
          }
        }
      } //for
    }
    getgrid(new_gridid)->on_objectenter(object);
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Scene::canenter() {
  __ENTER_FUNCTION
    if (getconfig_boolvalue(kConfigNetManagerIsActive)) {
      uint16_t netcount = netmanager_->getcount();
      if (netcount > kScenePlayerMax) return false;
    }
    if (getstatus() != kStatusRuning) return false;
    if (quittimer_.isstart()) return false;
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Scene::isfull() {
  __ENTER_FUNCTION
    bool result = false;
    if (getconfig_boolvalue(kConfigNetManagerIsActive)) {
      uint16_t netcount = netmanager_->getcount();
      if (netcount >= kScenePlayerMax) result = true;
    }
    return result;
  __LEAVE_FUNCTION
    return false;
}

int8_t Scene::entercheck(int16_t connectionid) {
  __ENTER_FUNCTION
    int8_t result = 1;
    if (getconfig_boolvalue(kConfigNetManagerIsActive)) {
      pf_net::connection::Base *connection = NULL;
      connection = netmanager_->getpool()->get(connectionid);
      if (NULL == connection) {
        Assert(false);
        return -1;
      }
      if (getstatus() != kStatusRuning) return 0;
      if (quittimer_.isstart()) return 0;
    }
    return result;
  __LEAVE_FUNCTION
    return -1;
}

void Scene::get_vrect_inradius(structure::vrect_t *vrect,
                               int32_t radius,
                               gridid_t gridid) {
  __ENTER_FUNCTION
    Assert(gridid != ID_INVALID);
    int32_t width = gridid % gridinfo_.width;
    int32_t height = gridid % gridinfo_.width;
    if (radius < 0) radius = 0;
    vrect->startx = width - radius;
    if (vrect->startx < 0) vrect->startx = 0;
    vrect->startz = height - radius;
    if (vrect->startz < 0) vrect->startz = 0;
    vrect->endx = width + radius;
    if (vrect->endx >= gridinfo_.width) vrect->endx = gridinfo_.width - 1;
    if (vrect->endz >= gridinfo_.height) vrect->endz = gridinfo_.height - 1;
  __LEAVE_FUNCTION
}

void Scene::mail_normal(structure::Object *player, 
                        const char *receivername, 
                        const char *content) {
  USE_PARAM(player);
  USE_PARAM(receivername);
  USE_PARAM(content);
  //do nothing
}
  
void Scene::mail_system(const char *receivername, const char *content) {
  USE_PARAM(receivername);
  USE_PARAM(content);
  //do nothing
}
   
void Scene::mail_script(const char *receivername, 
                        int32_t param0, 
                        int32_t param1, 
                        int32_t param2, 
                        int32_t param3) {
  USE_PARAM(receivername);
  USE_PARAM(param0);
  USE_PARAM(param1);
  USE_PARAM(param2);
  USE_PARAM(param3);
  //do nothing
}

bool Scene::remove_allobject() {
  return true;
}

} //namespace scene
