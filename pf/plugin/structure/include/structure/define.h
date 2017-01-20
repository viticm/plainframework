/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id define.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm@126.com>/viticm.ti@gmail.com
 * @date 2015/03/17 14:42
 * @uses structure define file
 *       cn: 数据结构定义文件，主要是结构体和宏的定义
*/
#ifndef STRUCTURE_DEFINE_H_
#define STRUCTURE_DEFINE_H_

#include "structure/config.h"
#include "pf/net/socket/inputstream.h"
#include "pf/net/socket/outputstream.h"

#define CAMP_INVALID (0xFF)
#define NPC_NAME_LENGTH (32)
#define NPC_TITLE_LENGTH (32)
#define BUFF_IDCOUNT_MAX (20)
#define NPC_SHOP_MAX (4)
#define STRUCTURE_OBJECTLIST_MAX (160)
#define AI_TARGETGROUP_COUNTMAX (32)

typedef uint32_t objectid_t; //对象ID
typedef int32_t gridid_t; //格子ID
typedef int32_t scriptid_t; //脚本ID
typedef int64_t guid_t; //唯一ID
typedef int32_t teamid_t; //队伍ID
typedef uint8_t campid_t; //阵营ID
typedef int16_t sceneid_t; //场景ID

namespace structure {

typedef enum object_type_enum {
  kObjectTypeInvalid = -1, //无效的
  kObjectTypeHuman, //玩家
  kObjectTypeMonster, //怪物或NPC
  kObjectTypeNumber, 
} object_type_t; //对象类型

typedef enum message_type_enum {
  kMessageTypeInvalid = -1,
  kMessageTypeChat, //聊天
  kMessageTypeTips, //提示
} message_type_t; //消息类型

typedef enum object_idtype_enum {
  kObjectIdTypeInvalid = -1,
  kObjectIdTypeNormal,
  kObjectIdTypeHuman,
  kObjectIdTypeNumber,
} object_idtype_t;

struct PF_PLUGIN_API position_struct {
  float x;
  float z;
  position_struct();
  position_struct(float _x, float _z);
  void clear();
  position_t &operator = (position_t const &position);
  position_t &operator = (const position_t *position);
  bool operator == (position_t &position);
  bool operator != (position_t &position);
  bool operator == (const position_t &position);
  static uint32_t getsize();
  static uint32_t get_sizemax();
  bool write(pf_net::socket::OutputStream &outputstream);
  bool read(pf_net::socket::InputStream &inputstream);
}; //坐标

struct PF_PLUGIN_API object_struct {
  position_t position; //位置
  float direction; //方向
  int32_t viewgroup; //对象的可视组
  void *data; //数据指针
  object_struct();
  virtual void clear();
}; //对象基础结构

struct PF_PLUGIN_API objectlist_node_struct {
  Object *node;
  objectlist_node_t *next;
  objectlist_node_t *prev;
  objectlist_node_struct();
  objectlist_node_struct(Object *_node);
  void clear();
}; //对象链表

struct PF_PLUGIN_API campdata_struct {
  enum {
    kSize = sizeof(campid_t) + sizeof(uint8_t) + sizeof(int32_t) * 2,
  };
  campid_t id;
  uint8_t pkmode;
  int32_t reserve1;
  int32_t reserve2;
  campdata_struct();
  void clear();
  campid_t getid() const;
  uint8_t get_pkmode() const;
  void set_pkmode(uint8_t mode);
  bool operator == (const campdata_t &in);
  bool operator != (const campdata_t &in);
  campdata_t &operator = (const campdata_t &in);
  uint32_t getsize() const;
  bool write(pf_net::socket::OutputStream &outputstream);
  bool read(pf_net::socket::InputStream &inputstream);
}; //阵营

typedef struct PF_PLUGIN_API vrect_struct {
  int32_t startx;
  int32_t startz;
  int32_t endx;
  int32_t endz;
  vrect_struct();
  void clear();
  bool iscontinue(int32_t x, int32_t z);
} vrect_t; //特殊的RECT结构

typedef struct PF_PLUGIN_API objectlist_id_struct {
  enum {kListSizeMax = 2048,};
  int32_t count;
  objectid_t ids[kListSizeMax];
  objectlist_id_struct();
  void clear();
  uint32_t getsize() const;
  bool write(pf_net::socket::OutputStream &outputstream);
  bool read(pf_net::socket::InputStream &inputstream);
} objectlist_id_t; //对象ID列表结构

typedef struct PF_API dueldata_struct dueldata_t;

struct dueldata_struct {
  objectid_t opponent_objectid;
  guid_t opponent_guid;
  uint8_t regionid;
  sceneid_t sceneid;
  uint32_t starttime;
  uint8_t openlevel;
  dueldata_struct();
  void clear();
  dueldata_t &operator = (const dueldata_t &in);
  bool operator == (const dueldata_t &in);
  bool operator != (const dueldata_t &in);
  uint32_t getsize() const;
  bool write(pf_net::socket::OutputStream &outputstream);
  bool read(pf_net::socket::InputStream &inputstream);
}; //决斗数据

typedef struct human_campdata_struct human_campdata_t;

struct PF_PLUGIN_API human_campdata_struct : public campdata_struct {
  objectid_t objectid;
  uint8_t level;
  sceneid_t sceneid;
  uint8_t scene_safelevel;
  uint8_t scene_restrictivemode;
  guid_t guid;
  teamid_t teamid;
  uint8_t saferegion_id;
  campid_t saferegion_campid;
  dueldata_t dueldata;
  human_campdata_struct();
  human_campdata_struct(campdata_t *in);
  void clear();
  human_campdata_t &operator = (const campdata_t &in);
  human_campdata_t &operator = (const human_campdata_t &in);
}; //玩家阵营数据结构

struct PF_PLUGIN_API objectlist_struct {
  Object *list[STRUCTURE_OBJECTLIST_MAX];
  uint16_t count;
  objectlist_struct();
  void clear();
}; //对象列表结构

typedef struct PF_PLUGIN_API objectsingle_manager_init_struct {
  uint32_t length;
  void *scene;
  uint32_t logicinterval;
  uint32_t currenttime;
  objectsingle_manager_init_struct();
  void clear();
} objectsingle_manager_init_t;

typedef struct PF_PLUGIN_API objectmanager_init_struct {
  uint32_t initcount[kObjectIdTypeNumber];
  objectmanager_init_struct();
  void clear();
} objectmanager_init_t;

typedef struct PF_PLUGIN_API aidata_struct aidata_t;
struct aidata_struct {
  int16_t eventsource;
  int32_t eventparam;
  int32_t targetgroup_count;
  structure::Object *targetgroup[AI_TARGETGROUP_COUNTMAX];
  aidata_struct();
  void clear();
};

/**
typedef enum actor_logicstatus_enum {
  kActorLogicStatusInvalid = -1,
  kActorLogicStatusIdle, 
  kActorLogicStatusMove,
  kActorLogicStatusUseSkill,
  kActorLogicStatusUseAbility,
} actor_logicstatus_t; //角色逻辑状态

typedef struct object_actor_struct object_actor_t;
struct object_actor_struct() : public object_struct {
  actor_logicstatus_t logicstatus;
  object_actor_struct() {
    logicstatus = kActorLogicStatusInvalid;
  };
  virtual void clear() {
    logicstatus = kActorLogicStatusInvalid;
    object_struct::clear();
  };
}; //角色基础数据结构

typedef struct object_monster_struct object_monster_t;
struct object_monster_struct : public object_actor_struct {
  guid_t guid; //唯一ID
  uint32_t dataid; //数据ID
  char name[NPC_NAME_LENGTH]; //名称
  char title[NPC_TITLE_LENGTH]; //称号
  scriptid_t scriptid; //脚本ID
  int32_t respawntime; //刷新时间
  uint32_t groupid; //组ID
  uint32_t teamid; //队伍ID
  int32_t baseAI; //基础AI
  int32_t extend_AIscript; //扩展AI脚本
  int32_t patrolid; //巡逻ID
  int32_t shop[NPC_SHOP_MAX]; //商店ID
  uint32_t leaderid; //队长ID
  bool ispet; //是否为宠物
  uint32_t livetime; //存活时间
  guid_t objectguid; //对象GUID
  campid_t campid; //阵营ID
  int32_t questid; //任务ID
  int32_t level; //等级
  int32_t is_changemode_open; //模型改变是否激活
  int32_t buffid[BUFF_IDCOUNT_MAX]; //身上的BUFF
  char changedname[NPC_NAME_LENGTH]; //改变时的名字
  char changedtitle[NPC_TITLE_LENGTH]; //改变时的称号
  object_monster_struct() {
    clear(false);
  };
  void clear(bool all = true) {
    guid = ID_INVALID;
    dataid = ID_INVALID;
    name[0] = 0;
    title[0] = 0;
    respawntime = 60000;
    groupid = ID_INVALID;
    teamid = ID_INVALID;
    baseAI = 0;
    extend_AIscript = ID_INVALID;
    patrolid = ID_INVALID;
    leaderid = ID_INVALID;
    ispet = false;
    livetime = -1;
    objectguid = ID_INVALID;
    campid = CAMP_INVALID;
    questid = ID_INVALID;
    level = -1;
    is_changemode_open = 0;
    changedname[0] = 0;
    changedtitle[0] = 0;
    for (int32_t i = 0; i < NPC_SHOP_MAX; ++i)
      shop[i] = ID_INVALID;
    for (int32_t i = 0; i < BUFF_IDCOUNT_MAX; ++i)
      buffid[i] = ID_INVALID;
    if (all) object_actor_struct::clear();
  };
}; //怪物基础数据结构，希望以后能统一角色和怪物配置
**/

}; //namespace structure

#endif //STRUCTURE_DEFINE_H_
