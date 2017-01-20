/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id define.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm@126.com>/<viticm.ti@gmail.com>
 * @date 2015/03/23 15:46
 * @uses scene define file
 *       cn: 场景插件定义文件，主要是结构体的定义
**/
#ifndef SCENE_DEFINE_H_
#define SCENE_DEFINE_H_

#include "scene/config.h"
#include "pf/net/config.h"

namespace scene {

/* enum { */
typedef enum type_enum {
  kTypeInvalid = -1,
  kTypeLogic, //逻辑场景
  kTypeFuben,
  kTypeCity,
  kTypeNumber,
} type_t; //场景类型

typedef enum status_enum {
  kStatusSleep = 0, //休眠
  kStatusSelect, //选择中
  kStatusLoading, //加载中
  kStatusInit, //初始化中
  kStatusRuning, //正常运行
} status_t; //场景状态

enum {
  kConfigMapIsActive = 1, //场景地图数据是否激活
  kConfigNetManagerIsActive, //场景网络管理器是否激活
  kConfigObjectManagerIsActive, //场景对象管理器是否激活
  kConfigHumanManagerIsActive, //场景玩家管理器是否激活
  kConfigMonsterManagerIsActive, //场景怪物管理器是否激活
  kConfigTimerDoingManagerIsActive, //场景定时执行器是否激活
  kConfigTimersIsActive, //场景定时器是否激活
  kConfigChatPipelineIsActive, //场景聊天管道是否激活
  kConfigRecycleConnectionManagerIsActive, //连接回收器是否激活
  kConfigDropPositionManagerIsActive, //掉落管理器是否激活
  kConfigRegionManagerIsActive, //区域管理器是否激活
  kConfigPKResionManagerIsActive, //安全区域管理器是否激活
  kConfigFubenIsActive, //是否支持副本
  kConfigVariableManagerIsActive, //变量管理器是否激活
  kConfigScriptIsActive, //脚本是否激活
  kConfigScriptMutliModeIsActive, //脚本多数据模式是否激活，即每个场景的脚本数据独立
}; //场景配置项

typedef enum performance_type_struct {
  kPerformanceTypeTickBefore = 0,
  kPerformanceTypeTickLogic,
  kPerformanceTypeTickSelect,
  kPerformanceTypeTickProcessExceptions,
  kPerformanceTypeTickProcessInputs,
  kPerformanceTypeTickProcessCommands,
  kPerformanceTypeTickProcessCacheCommands,
  kPerformanceTypeTickProcessOutputsBHB,
  kPerformanceTypeTickProcessOutputsAHB,
  kPerformanceTypeTickHeartBeat,
  kPerformanceTypeTickHeartBeatCloseLogic,
  kPerformanceTypeTickHeartBeatTimer,
  kPerformanceTypeTickHeartBeatNetManager,
  kPerformanceTypeTickHeartBeatHumanManager,
  kPerformanceTypeTickHeartBeatMonsterManager,
  kPerformanceTypeTickHeartBeatObjectManager,
  kPerformanceTypeTickHeartBeatChatPipeline,
  kPerformanceTypeTickHeartBeatTimers,
  kPerformanceTypeTickHeartBeatRecycleConnectionManager,
  kPerformanceTypeTickHeartBeatVariableManager,
  kPerformanceTypeNumber,
} performance_type_t; //会影响场景性能的类型

typedef enum scanerturn_enum {
  kScanerTurnContinue = 0,
  kScanerTurnBreak,
  kScanerTurnReturn,
  kScanerTurnNumber,
} scanerturn_t; //扫描器的结果

enum {
  kBroadCastRadiusMin = 2,
  kBroadCastRadiusMax = 5,
  kScenePlayerMax = 2048,
}; //场景的一些定义

/* } enum */

/* struct { */
typedef struct PF_PLUGIN_API rect_struct {
  float left;
  float right;
  float top;
  float bottom;
  rect_struct();
  void clear();
  bool iscontain(float x, float z) const;
} rect_t; //场景矩形结构体

typedef struct PF_PLUGIN_API region_struct {
  uint32_t id;
  uint32_t scriptid;
  rect_t rect;
  region_struct();
  void clear();
  bool iscontain(float x, float z) const;
} region_t; //区域结构

typedef struct PF_PLUGIN_API chatitem_struct {
  pf_net::packet::Base *packet;
  objectid_t sourceid;
  objectid_t targetid;
  chatitem_struct();
  void clear();
} chatitem_t; //场景信息结构

typedef struct PF_PLUGIN_API regionconfig_struct {
  char filename[FILENAME_MAX];
  region_t *region;
  int32_t count;
  regionconfig_struct();
  ~regionconfig_struct();
  void clear();
} regionconfig_t; //区域配置文件

typedef struct PF_PLUGIN_API load_struct {
  char map_filename[FILENAME_MAX];
  char monster_filename[FILENAME_MAX];
  char region_filename[FILENAME_MAX];
  char pkregion_filename[FILENAME_MAX];
  bool is_pkregion_exist;
  load_struct();
  void clear();
} load_t; //场景加载结构，外部可以继承扩展

typedef struct PF_PLUGIN_API initdata_struct {
  int8_t datatype;
  initdata_struct();
  void clear();
  bool write(pf_net::socket::OutputStream &outputstream);
  bool read(pf_net::socket::InputStream &inputstream);
  uint32_t getsize() const;
} initdata_t; //场景初始化结构数据

typedef struct PF_PLUGIN_API variable_struct {
  int32_t values[SCENE_VARIABLE_MAX];
  variable_struct();
  void clear();
} variable_t; //场景变量结构

typedef struct PF_PLUGIN_API variableopt_struct {
  variable_t variable;
  bool dirty;
  variableopt_struct();
  void clear();
  int32_t get(int32_t index);
  void set(int32_t index, int32_t value);
} variableopt_t; //场景变量操作结构

typedef struct PF_PLUGIN_API pkregion_struct {
  int32_t id;
  int32_t type;
  campid_t base_campid;
  rect_t rect;
  pkregion_struct();
  void clear();
  bool iscontain(float x, float z) const;
} pkregion_t;

typedef struct PF_PLUGIN_API pkregion_config_struct {
  char filename[FILENAME_MAX];
  pkregion_t *pkregion;
  int32_t count;
  sceneid_t sceneid;
  pkregion_config_struct();
  ~pkregion_config_struct();
  void clear();
} pkregion_config_t; //PK区域配置结构

typedef struct fuben_data_struct fuben_data_t;
struct PF_PLUGIN_API fuben_data_struct {
  sceneid_t source_sceneid;
  int32_t worldid; //世界ID，表示玩家归属服务器
  uint32_t nouser_closetime;
  uint32_t scenetimer;
  int32_t param[SCENE_FUBEN_DATA_PARAM_MAX];
  teamid_t teamid;
  guid_t teamleader;
  guid_t memberguid[SCENE_FUBEN_DATA_COPYMEMBER_MAX];
  sceneid_t bakscene;
  structure::position_t bakposition;
  bool playerdisconnet; //记录是否有玩家下线
  fuben_data_struct();
  void clear();
  fuben_data_t &operator = (const fuben_data_t &data);
  bool is_havemember(guid_t guid);
  void add_memberguid(guid_t guid);
}; //副本数据基本结构

typedef struct PF_PLUGIN_API fuben_select_struct {
  load_t sceneload;
  int8_t scene_datatype;
  fuben_data_t fuben_data;
  fuben_select_struct();
  void clear();
  void set_sceneload(const char *filename);
} fuben_select_t;

typedef struct PF_PLUGIN_API performance_struct {
  uint32_t ticks_[kPerformanceTypeNumber];
  sceneid_t sceneid;
  performance_struct();
  void clear();
} performance_t; //场景性能记录结构体

typedef struct config_struct {
  char filename[FILENAME_MAX];
  load_t load;
  config_struct();
} config_t; //基本场景配置

typedef struct PF_PLUGIN_API clientres_struct {
  char nav_filename[FILENAME_MAX];
  int32_t id;
  clientres_struct();
} clientres_t; //客户端资源

typedef struct PF_PLUGIN_API monsterconfig_struct {
  char filename[FILENAME_MAX];
  structure::object_t *initdata;
  int32_t count;
  monsterconfig_struct();
  ~monsterconfig_struct();
  void clear();
} monsterconfig_t; //怪物配置结构

typedef struct PF_PLUGIN_API setting_struct {
  sceneid_t id;
  bool isactive;
  char name[FILENAME_MAX];
  char filename[FILENAME_MAX];
  int32_t serverid;
  uint8_t type;
  int32_t threadindex;
  int32_t clientres;
  uint8_t securitylevel;
  uint8_t restrictivemode;
  uint8_t safelevel;
  bool punish;
  int32_t defaultx;
  int32_t defaultz;
  campid_t default_campid;
  int32_t broadcast_radius;
  setting_struct();
  void clear();
} setting_t; //单场景配置基本结构

typedef struct PF_PLUGIN_API settings_struct {
  setting_t *scenes;
  uint16_t count;
  int32_t timercount_max;
  int32_t hash[SCENE_MAX];
  settings_struct();
  ~settings_struct();
} settings_t; //所有场景配置基本结构

typedef struct PF_PLUGIN_API scaner_init_struct {
  Scene *scene;
  gridid_t gridid;
  int32_t gridradius;
  bool scanhuman;
  scaner_init_struct();
  void clear();
} scaner_init_t; //扫描器初始化结构

typedef struct PF_PLUGIN_API gridinfo_sturct {
  uint16_t width;
  uint16_t height;
  uint16_t size;
  gridinfo_sturct();
  void clear();
} gridinfo_t; //格子信息

/* } struct */

}; //namespace scene

#endif //SCENE_DEFINE_H_
