/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id scene.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm@126.com>/<viticm.ti@gmail.com>
 * @date 2015/03/19 21:54
 * @uses scece manager class
 *       cn: 场景管理器，所有场景有关的管理器以及初始化工作放在这里
 *       主要的功能有：场景初始化、场景管理、玩家数量管理、场景配置、区域配置、
 *       非安全区域配置、怪物配置等接口。
**/
#ifndef SCENE_MANAGER_SCENE_H_
#define SCENE_MANAGER_SCENE_H_

#include "scene/manager/config.h"

namespace scene {

namespace manager {

class PF_PLUGIN_API Scene {

 public:
   Scene();
   ~Scene();

 public:
   bool init();
   bool init_clientres();

 public:
   scene::Scene *get(sceneid_t id);
   uint32_t getcount() const;
   int32_t get_resid(sceneid_t id);
   void set_resid(sceneid_t id, int32_t resid);
   sceneid_t getid_by_threadid(uint64_t id);
   bool add(scene::Scene *scene);
   bool remove(sceneid_t id);
   void set_playercount(sceneid_t id, int32_t count);
   int32_t get_playercount(sceneid_t id) const;
   void set_total_playercount(int32_t count);
   int32_t get_totalplayercount() const;
   bool set_loaddata(const char *filename, scene::load_t &load);
   int32_t get_resid_by_navname(const char *navname);
   const scene::setting_t *getsetting_byclient_resid(int32_t resid);
   bool add_resnav(const char *navname, int32_t resid, int32_t scenetype);
   void set_rootpath(const char *path);
   void get_fullpath(char *save, const char *filename);
   void set_settings(scene::settings_t *settings);
   scene::settings_t *get_settings();
   void set_serverid(int32_t serverid);

 public: //config
   scene::setting_t *get_setting(sceneid_t sceneid);
   scene::monsterconfig_t *get_monsterconfig(const char *filename);
   scene::monsterconfig_t *get_empty_monsterconfig();
   scene::regionconfig_t *get_regionconfig(const char *filename);
   scene::regionconfig_t *get_empty_regionconfig();
   scene::pkregion_config_t *get_pkregion_config(const char *filename);
   scene::pkregion_config_t *get_empty_pkregion_config();
   scene::config_t *getconfig(const char *filename);
   scene::config_t *getconfig_by_navname(const char *navname);
   scene::config_t *get_empty_config();

 protected:
   scene::Scene *scenes_[SCENE_MAX]; //场景指针数组
   int32_t playercounts_[SCENE_MAX]; //场景玩家数量数组
   int32_t total_playercount_; //所有场景的玩家总数
   int32_t count_; //场景数量
   clientres_t clientres_[SCENE_MAX]; //客户端资源配置
   int32_t clientres_ids_[SCENE_MAX]; //客户端资源ID
   scene::monsterconfig_t monsterconfig_[SCENE_MAX]; //怪物配置
   scene::regionconfig_t regionconfig_[SCENE_MAX]; //区域配置
   scene::config_t config_[SCENE_MAX]; //场景配置
   scene::pkregion_config_t pkregion_config_[SCENE_MAX]; //安全区域配置 
   scene::manager::Thread *threadmanager_; //线程管理器
   scene::manager::Fuben *fubenmanager_; //副本管理器
   char rootpath_[FILENAME_MAX];
   scene::settings_t *settings_; //场景配置
   int32_t serverid_; //服务器ID

};

}; //namespace manager

}; //namespace scene

#endif //SCENE_MANAGER_SCENE_H_
