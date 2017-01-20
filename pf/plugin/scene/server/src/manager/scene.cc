#include "pf/base/string.h"
#include "pf/file/ini.h"
#include "pf/base/log.h"
#include "scene/thread.h"
#include "scene/scene.h"
#include "scene/manager/thread.h"
#include "scene/manager/scene.h"
#include "scene/manager/fuben.h"

namespace scene {

namespace manager {

Scene::Scene() {
  __ENTER_FUNCTION
    for (uint16_t i = 0; i < SCENE_MAX; ++i) {
      scenes_[i] = NULL;
      playercounts_[i] = 0;
      clientres_ids_[i] = ID_INVALID;
    }
    total_playercount_ = 0;
    threadmanager_ = NULL;
    fubenmanager_ = NULL;
    settings_ = NULL;
  __LEAVE_FUNCTION
}

Scene::~Scene() {
  __ENTER_FUNCTION
    for (uint16_t i = 0; i < SCENE_MAX; ++i) {
      SAFE_DELETE(scenes_[i]);
    }
  __LEAVE_FUNCTION
}

bool Scene::init() {
  __ENTER_FUNCTION
    bool result = false;
    Assert(settings_);
    uint16_t count = settings_->count;
    Assert(count <= SCENE_MAX);
    init_clientres();
    for (uint16_t i = 0; i < count; ++i) {
      sceneid_t sceneid = settings_->scenes[i].id;
      Assert(sceneid < SCENE_MAX);
      if (serverid_ != settings_->scenes[i].serverid) continue;
      if (!settings_->scenes[i].isactive) continue;
      scene::Scene *scene = new scene::Scene;
      Assert(scene);
      scene::Thread *thread = dynamic_cast<scene::Thread *>(
        threadmanager_->getpool()->get_byindex(settings_->scenes[i].threadindex));
      Assert(thread);
      scene->set_scenemanager(this);
      scene->init(sceneid);
      thread->addscene(scene);
      scene->settype(settings_->scenes[i].type);
      scene->set_safelevel(settings_->scenes[i].safelevel);
      scene->set_restrictivemode(settings_->scenes[i].restrictivemode);
      scene->setpunish(settings_->scenes[i].punish);
      scene->set_canenter_position(settings_->scenes[i].defaultx, 
                                   settings_->scenes[i].defaultz);
      scene->set_broadcast_radius(settings_->scenes[i].broadcast_radius);
      switch (scene->gettype()) {
        case kTypeLogic: {
          load_t load;
          set_loaddata(settings_->scenes[i].filename, load);
          result = scene->load(&load);
          Assert(result);
          scene->setstatus(kStatusRuning);
          break;
        }
        case kTypeFuben: {
          set_loaddata(settings_->scenes[i].filename, scene->load_);
          pf_base::string::safecopy(scene->get_map_filename(), 
                                    scene->load_.map_filename, 
                                    sizeof(scene->load_.map_filename));
          scene->setstatus(kStatusSleep);
          fubenmanager_->addscene(sceneid, scene);
          break;
        }
        default:
          Assert(false);
          break;
      }
      result = add(scene);
      Assert(result);
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Scene::init_clientres() {
  __ENTER_FUNCTION
    uint16_t scenecount = settings_->count;
    Assert(scenecount <= SCENE_MAX);
    for (uint16_t i = 0; i <scenecount; ++i) {
      load_t load;
      set_loaddata(settings_->scenes[i].filename, load);
      bool result = add_resnav(load.map_filename, 
                               settings_->scenes[i].clientres, 
                               settings_->scenes[i].type);
      Assert(result);
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

scene::Scene *Scene::get(sceneid_t id) {
  __ENTER_FUNCTION
    scene::Scene *scene = NULL;
    if (id > 0 && id < SCENE_MAX) scene = scenes_[id];
    return scene;
  __LEAVE_FUNCTION
    return NULL;
}

uint32_t Scene::getcount() const {
  return count_;
}

int32_t Scene::get_resid(sceneid_t id) {
  __ENTER_FUNCTION
    int32_t resid = ID_INVALID;
    if (ID_INVALID == resid) return resid;
    scene::setting_t *setting = get_setting(id);
    if (setting != NULL) {
      if (kTypeFuben == setting->type) {
        resid = clientres_ids_[id];
      } else {
        resid = setting->clientres;
      }
    }
    return resid;
  __LEAVE_FUNCTION
    return ID_INVALID;
}

void Scene::set_resid(sceneid_t id, int32_t resid) {
  __ENTER_FUNCTION
    if (ID_INVALID == id) return;
    scene::setting_t *setting = get_setting(id);
    if (setting && kTypeFuben == setting->type)
      clientres_ids_[id] = resid;
  __LEAVE_FUNCTION
}

sceneid_t Scene::getid_by_threadid(uint64_t id) {
  __ENTER_FUNCTION
    sceneid_t sceneid = ID_INVALID;
    for (uint16_t i = 0; i < SCENE_MAX; ++i) {
      if (NULL == scenes_[i]) continue;
      if (id == scenes_[i]->threadid_) {
        sceneid = scenes_[i]->getid();
        break;
      }
    }
    return sceneid;
  __LEAVE_FUNCTION
    return ID_INVALID;
}

bool Scene::add(scene::Scene *scene) {
  __ENTER_FUNCTION
    if (NULL == scene) return false;
    sceneid_t sceneid = scene->getid();
    Assert(sceneid < SCENE_MAX);
    Assert(NULL == scenes_[sceneid]);
    scenes_[sceneid] = scene;
    ++count_;
    Assert(count_ < SCENE_MAX);
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool Scene::remove(sceneid_t id) {
  __ENTER_FUNCTION
    if (id < 0 || id >= SCENE_MAX) return false;
    if (NULL == scenes_[id]) return false;
    scenes_[id] = NULL;
    --count_;
    return true;
  __LEAVE_FUNCTION
    return false;
}

void Scene::set_playercount(sceneid_t id, int32_t count) {
  __ENTER_FUNCTION
    if (id < 0 || id >= SCENE_MAX) return;
    playercounts_[id] = count;
  __LEAVE_FUNCTION
}

void Scene::set_total_playercount(int32_t count) {
  total_playercount_ = count;
}

int32_t Scene::get_totalplayercount() const {
  return total_playercount_;
}

void Scene::set_rootpath(const char *path) {
  __ENTER_FUNCTION
    memset(rootpath_, 0, sizeof(rootpath_));
    pf_base::string::safecopy(rootpath_, path, sizeof(rootpath_));
  __LEAVE_FUNCTION
}

void Scene::get_fullpath(char *save, const char *filename) {
  __ENTER_FUNCTION
    snprintf(save, 
             FILENAME_MAX - 1, 
             "%s%s",
             rootpath_,
             filename);
  __LEAVE_FUNCTION
}

bool Scene::set_loaddata(const char *filename, scene::load_t &load) {
  __ENTER_FUNCTION
    using namespace pf_base::string;
    scene::config_t *config = getconfig(filename);
    if (NULL == config) {
      char temp[FILENAME_MAX] = {0};
      char map_filename[FILENAME_MAX] = {0};
      char monster_filename[FILENAME_MAX] = {0};
      char region_filename[FILENAME_MAX] = {0};
      char pkregion_filename[FILENAME_MAX] = {0};
      bool is_pkregion_exist = false;
      config = get_empty_config();
      if (NULL == config) return false;
      get_fullpath(temp, filename);
      pf_file::Ini ini(temp);
      ini.readstring("System", "navfile", map_filename, sizeof(map_filename));
      ini.readstring("System", 
                     "monsterfile", 
                     monster_filename, 
                     sizeof(monster_filename));
      ini.readstring(
          "System", "regionfile", region_filename, sizeof(region_filename));
      is_pkregion_exist = ini.read_existstring("System", 
                                               "pkregionfile", 
                                               pkregion_filename, 
                                               sizeof(pkregion_filename));
      load_t _load;
      _load.clear();
      safecopy(_load.map_filename, map_filename, sizeof(_load.map_filename));
      safecopy(_load.monster_filename, 
               monster_filename, 
               sizeof(_load.monster_filename));
      safecopy(_load.region_filename, 
               region_filename, 
               sizeof(_load.region_filename));
      safecopy(_load.pkregion_filename,
               pkregion_filename,
               sizeof(_load.pkregion_filename));
      _load.is_pkregion_exist = is_pkregion_exist;
      config->load = _load;
      safecopy(config->filename, filename, sizeof(config->filename));
    }
    load = config->load;
    return true;
  __LEAVE_FUNCTION
    return false;
}

int32_t Scene::get_resid_by_navname(const char *navname) {
  __ENTER_FUNCTION
    int32_t resid = ID_INVALID;
    if (NULL == navname || '\0' == navname[0]) return resid;
    for (uint16_t i = 0; i < SCENE_MAX; ++i) {
      if (0 == clientres_[i].nav_filename[0]) break;
      if (0 == strcmp(clientres_[i].nav_filename, navname)) {
        resid = clientres_[i].id;
        break;
      }
    }
    return resid;
  __LEAVE_FUNCTION
    return ID_INVALID;
}

const scene::setting_t *Scene::getsetting_byclient_resid(int32_t resid) {
  __ENTER_FUNCTION
    scene::setting_t *setting = NULL;
    if (resid < 0) return setting;
    for (uint16_t i = 0; i < SCENE_MAX; ++i) {
      setting = get_setting(static_cast<sceneid_t>(i));
      if (resid == setting->clientres) break;
    }
    return setting;
  __LEAVE_FUNCTION
    return NULL;
}

bool Scene::add_resnav(const char *navname, int32_t resid, int32_t scenetype) {
  __ENTER_FUNCTION
    bool result = false;
    if (kTypeFuben == scenetype) {
      for (uint16_t i = 0; i < SCENE_MAX; ++i) {
        if (0 == strcmp(clientres_[i].nav_filename, navname)) {
          result = true;
          break;
        }
      }
    }
    for (uint16_t i = 0; i < SCENE_MAX; ++i) { 
      if (0 == clientres_[i].nav_filename[0]) {
        pf_base::string::safecopy(clientres_[i].nav_filename, 
                                  navname, 
                                  sizeof(clientres_[i].nav_filename));
        clientres_[i].id = resid;
        result = true;
        break;
      }
    }
    return result;
  __LEAVE_FUNCTION
    return false;
}

void Scene::set_settings(scene::settings_t *settings) {
  settings_ = settings;
}

scene::settings_t *Scene::get_settings() {
  return settings_;
}

void Scene::set_serverid(int32_t serverid) {
  serverid_ = serverid;
}

scene::setting_t *Scene::get_setting(sceneid_t sceneid) {
  __ENTER_FUNCTION
    scene::setting_t *setting = NULL;
    if (NULL == settings_) return setting;
    if (sceneid < 0 || static_cast<uint16_t>(sceneid) > settings_->count) {
      SLOW_ERRORLOG(SCENE_MODULENAME,
                    "[scene.manager] (Scene::get_setting) scene id error"
                    " (%d)",
                    sceneid);
      return setting;
    }
    int32_t index = settings_->hash[sceneid];
    Assert(index != ID_INVALID);
    setting = &settings_->scenes[index];
    return setting;
  __LEAVE_FUNCTION
    return NULL;
}

scene::config_t *Scene::getconfig(const char *filename) {
  __ENTER_FUNCTION
    scene::config_t *config = NULL;
    if (NULL == filename || 0 == filename[0]) return config;
    for (uint16_t i = 0; i < SCENE_MAX; ++i) {
      if (0 == config_[i].filename[0]) continue;
      if (0 == strcmp(config_[i].filename, filename)) {
        config = &config_[i];
        break;
      }
    }
    return config;
  __LEAVE_FUNCTION
    return NULL;
}

scene::config_t *Scene::getconfig_by_navname(const char *navname) {
  __ENTER_FUNCTION
    scene::config_t *config = NULL;
    if (NULL == navname || 0 == navname[0]) return config;
    for (uint16_t i = 0; i < SCENE_MAX; ++i) {
      if (0 == config_[i].filename[0]) continue;
      if (0 == config_[i].load.map_filename[0]) continue;
      if (0 == strcmp(config_[i].load.map_filename, navname)) {
        config = &config_[i];
        break;
      }
    }
    return config;
  __LEAVE_FUNCTION
    return NULL;
}

scene::config_t *Scene::get_empty_config() {
  __ENTER_FUNCTION
    scene::config_t *config = NULL;
    for (uint16_t i = 0; i < SCENE_MAX; ++i) {
      if (0 == config_[i].filename[0]) {
        config = &config_[i];
        break;
      }
    }
    return config; 
  __LEAVE_FUNCTION
    return NULL;
}

scene::monsterconfig_t *Scene::get_monsterconfig(const char *filename) {
  __ENTER_FUNCTION
    scene::monsterconfig_t *monsterconfig = NULL;
    if (NULL == filename || 0 == filename[0]) return monsterconfig;
    for (uint16_t i = 0; i < SCENE_MAX; ++i) {
      if (0 == monsterconfig_[i].filename[0]) continue;
      if (0 == strcmp(monsterconfig_[i].filename, filename)) {
        monsterconfig = &monsterconfig_[i];
        break;
      }
    }
    return monsterconfig;
  __LEAVE_FUNCTION
    return NULL;
}
   
scene::monsterconfig_t *Scene::get_empty_monsterconfig() {
  __ENTER_FUNCTION
    scene::monsterconfig_t *monsterconfig = NULL;
    for (uint16_t i = 0; i < SCENE_MAX; ++i) {
      if (0 == monsterconfig_[i].filename[0]) {
        monsterconfig = &monsterconfig_[i];
        break;
      }
    }
    return monsterconfig; 
  __LEAVE_FUNCTION
    return NULL;
}
   
scene::regionconfig_t *Scene::get_regionconfig(const char *filename) {
  __ENTER_FUNCTION
    scene::regionconfig_t *regionconfig = NULL;
    if (NULL == filename || 0 == filename[0]) return regionconfig;
    for (uint16_t i = 0; i < SCENE_MAX; ++i) {
      if (0 == regionconfig_[i].filename[0]) continue;
      if (0 == strcmp(regionconfig_[i].filename, filename)) {
        regionconfig = &regionconfig_[i];
        break;
      }
    }
    return regionconfig;
  __LEAVE_FUNCTION
    return NULL;
}
   
scene::regionconfig_t *Scene::get_empty_regionconfig() {
  __ENTER_FUNCTION
    scene::regionconfig_t *regionconfig = NULL;
    for (uint16_t i = 0; i < SCENE_MAX; ++i) {
      if (0 == regionconfig_[i].filename[0]) {
        regionconfig = &regionconfig_[i];
        break;
      }
    }
    return regionconfig; 
  __LEAVE_FUNCTION
    return NULL;
}

scene::pkregion_config_t *Scene::get_pkregion_config(const char *filename) {
  __ENTER_FUNCTION
    scene::pkregion_config_t *pkregionconfig = NULL;
    if (NULL == filename || 0 == filename[0]) return pkregionconfig;
    for (uint16_t i = 0; i < SCENE_MAX; ++i) {
      if (0 == pkregion_config_[i].filename[0]) continue;
      if (0 == strcmp(pkregion_config_[i].filename, filename)) {
        pkregionconfig = &pkregion_config_[i];
        break;
      }
    }
    return pkregionconfig;
  __LEAVE_FUNCTION
    return NULL;
}
   
scene::pkregion_config_t *Scene::get_empty_pkregion_config() {
  __ENTER_FUNCTION
    scene::pkregion_config_t *pkregionconfig = NULL;
    for (uint16_t i = 0; i < SCENE_MAX; ++i) {
      if (0 == pkregion_config_[i].filename[0]) {
        pkregionconfig = &pkregion_config_[i];
        break;
      }
    }
    return pkregionconfig; 
  __LEAVE_FUNCTION
    return NULL;
}

} //namespace manager

} //namespace scene
