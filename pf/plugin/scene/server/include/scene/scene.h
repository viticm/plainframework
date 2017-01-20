/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id scene.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm@126.com>/viticm.ti@gmail.com
 * @date 2015/03/16 16:43
 * @uses server scene base class
 *       cn: 服务器场景基础类，包含基本的场景功能
 *       这里整理了大部分场景应有的功能，但是有些功能可能根据不同游戏会有不同的表现
 *       所以在这里只做最简单的处理
 *
 *       在这里场景所做的核心功能有：
 *       1、场景中对象的管理（玩家和怪物）
 *       2、场景中消息的处理（聊天、广播、移动等）
 *       3、场景中副本的管理
 *       4、场景中对象的查询
 *       5、场景中定时或事件的处理
**/
#ifndef SCENE_SCENE_H_
#define SCENE_SCENE_H_

#include "scene/config.h"
#include "pf/net/packet/base.h"
#include "pf/net/connection/base.h"
#include "pf/base/tinytimer.h"
#include "pf/script/lua/interface.h"
#include "pf/base/hashmap/template.h"
#include "pf/util/random.h"
#include "scene/define.h"
#include "scene/grid.h"

#define SCENE_SCRIPTID 888888

namespace scene {

class PF_PLUGIN_API Scene {

 public:
   Scene();
   virtual ~Scene();

 public:
   pf_net::packet::Base *packet_newplayer_; //新入玩家包
   pf_net::packet::Base *packet_newplayer_move_; //玩家移动
   pf_net::packet::Base *packet_newplayer_death_; //玩家死亡或者消失
   pf_net::packet::Base *packet_newmonster_; //新入怪物
   pf_net::packet::Base *packet_newmonster_move_; //怪物移动
   pf_net::packet::Base *packet_newmonster_death_; //怪物死亡或消失
   pf_net::packet::Base *packet_deleteobject_; //对象删除

 public:
   uint64_t threadid_; //线程ID
   load_t load_; //加载场景的数据
   char map_filename_[FILENAME_MAX]; //场景地图文件名 nav文件名
   int32_t clientres_; //客户端资源ID
   performance_t performance_; //性能记录数据
   //xparam_t xparam_; //去掉此构造，如果有需要再弄，用于UICommand指令发送
   structure::objectlist_t humanlist_; //玩家列表
   structure::objectlist_t monsterlist_; //怪物列表
   structure::objectlist_id_t objectlist_id_; //对象ID列表
   uint32_t fighttime_; //战斗时间
   uint32_t tick_; //
   uint32_t tickend_;
   bool detectlock_; //是否检测死锁
   Thread *thread_; //线程指针
   uint32_t backuptime_; //
   uint32_t starttime_;
   uint32_t currenttime_;
   uint32_t last_frametime_;
   uint32_t currentframes_; //当前帧数
   int32_t currenttimes_;
   int32_t lastFPS_; //上一次的帧率
   int32_t money_; //钱
   bool is_fristinit_;
   initdata_t initdata_; //初始化数据
   fuben_data_t *fuben_data_; //副本数据
   pf_script::lua::Interface *luainterface_; //PF日后会支持其他的脚本，暂时写死
   int8_t gridsize_; //格子的大小

 public: //性能记录变量，时间
   uint32_t costtime_input_;
   uint32_t costtime_output1_;
   uint32_t costtime_output2_;
   uint32_t costtime_process_;
   uint32_t costtime_cache_;
   uint32_t costtime_heartbeat_;
 
 public:
   //id和配置文件名为场景管理器控制
   virtual bool load(load_t *load);
   virtual bool init(sceneid_t id);
   virtual bool tick(); //逻辑接口，被线程定时调用
   virtual bool statuslogic(); //控制当前场景状态的逻辑接口
   virtual bool status_falselogic(); //
   virtual bool heartbeat(); //心跳逻辑

 public:
   bool process_cachecommand(); //消息缓存
   bool packet_send(pf_net::packet::Base *packet, 
                    int16_t connectionid, 
                    uint32_t flag = kPacketFlagNone);

 public: //场景事件，考虑使用事件系统处理
   bool oninit(); //初始化完成
   void ontimer(uint32_t time); //定时器事件
   void onquit(); //退出前场景
   void onclose(); //场景关闭
   virtual void onenter(pf_net::connection::Base *connection); //进入
   virtual void ondisconnect(pf_net::connection::Base *connection); //断线
   virtual void onleave(pf_net::connection::Base *connection); //离开
   void onnotify(sceneid_t sceneid); //通知

 public:
   inline uint8_t getstatus() const;
   inline void setstatus(uint8_t status);
   bool close();

 public:
   inline scene::Map *getmap();
   inline scene::manager::Net *get_netmanager();
   inline structure::manager::Object *get_objectmanager();
   inline structure::manager::ObjectSingle *get_humanmanager();
   inline structure::manager::ObjectSingle *get_monstermanager();
   inline scene::manager::TimerDoing *get_timerdoing_manager();
   inline Timer *gettimer();
   inline ChatPipeline *get_chatpipeline();
   inline gridinfo_t *get_gridinfo();
   inline pf_script::lua::Interface *get_luainterface();
   inline scene::manager::RecycleConnection *get_recycleconnection_manager();
   inline scene::manager::DropPosition *get_dropposition_manager();
   inline scene::manager::Region *get_regionmanager();
   inline scene::manager::Variable *get_variablemanager();
   inline scene::manager::Scene *get_scenemanager();
   inline void set_scenemanager(scene::manager::Scene *scenemanager);
   
 public: //config
   void registerconfig(int32_t key, bool value);
   bool setconfig(int32_t key, bool value);
   bool getconfig_boolvalue(int32_t key);

 public:
   inline bool isinit() const;
   inline sceneid_t getid() const;

 public: //扫描和广播
   bool scan(Scaner *scaner);
   //以position点为中心，范围为range米内的humanlist
   bool scanhuman(structure::position_t *position, 
                  float range, 
                  structure::objectlist_t *list);
   
   //向当前场景内发送广播消息，广播的中心点位置为广播对象所在位置
   //广播距离为5 * 5
   //如果sendme设置为真，则消息同时也发给对象位置所在玩家
   bool broadcast(pf_net::packet::Base *packet, 
                  structure::Object *object, 
                  bool sendme = false,
                  bool lockme_only = false);
   
   //场景广播
   bool broadcast_scene(pf_net::packet::Base *packet);
   
   //向gridid所在区域为中心广播一个消息
   //范围为gridid所在区域以及周围N圈grid
   //N为配置好的信息
   bool broadcast(pf_net::packet::Base *packet, gridid_t gridid);
   
   //广播聊天消息时使用
   //gridid所在区域为中心广播一个消息
   //范围为gridid所在区域以及周围N圈grid
   //N为配置好的信息
   //gridid为ID_INVALID时，全场景广播
   bool broadcast_chat(pf_net::packet::Base *packet, gridid_t gridid);

   bool scanhuman(gridid_t gridid, 
                  uint32_t radius_gridcount, 
                  structure::objectlist_t *list);

   //gridid_A为中心以gridradius为阶(1为3*3 2为5*5)确定一个矩形区域A
   //gridid_B为中心以gridradius为阶(1为3*3 2为5*5)确定一个矩形区域B
   //求得区域A和区域B交集中的玩家列表
   bool scanhuman(gridid_t gridid_A, 
                  gridid_t gridid_B, 
                  uint32_t gridradius, 
                  structure::objectlist_t *list);
   //gridid为中心gridradius为阶(1为3*3 2为5*5)确定一个矩形区域中的对象列表
   bool scanobject(gridid_t gridid, 
                   uint32_t gridradius, 
                   structure::objectlist_t *list);

   //gridid_A为中心以gridradius为阶(1为3*3 2为5*5)确定一个矩形区域A
   //gridid_B为中心以gridradius为阶(1为3*3 2为5*5)确定一个矩形区域B
   //求得区域A和区域B交集中的对象列表列表
   bool scanobject(gridid_t gridid_A,
                   gridid_t gridid_B,
                   uint32_t gridradius,
                   structure::objectlist_t *list);

   //向对象列表中发送消息
   bool sendpacket(pf_net::packet::Base *packet, structure::objectlist_t *list);
   //同步创建时的属性
   bool sendpacket_createattr(structure::Object *object, 
                              structure::objectlist_t *list);


 public: //通用接口
   
   //对象相关
   bool objectenter(structure::Object *object);
   void objectleave(structure::Object *object, 
                    pf_net::packet::Base *packet = NULL);
   virtual structure::Object *objectnew(int8_t type, int32_t viewgroup);
   virtual void objectdelete(structure::Object *object);

   //区域相关
   bool regionregister(const region_t *region);
   const region_t *regioncheck(structure::Object *object);
   gridid_t calc_gridid(const structure::position_t *position);

   //格子对象
   bool objectgrid_register(structure::Object *object, 
                            gridid_t gridid, 
                            structure::Object *other);
   bool objectgrid_unregister(structure::Object *object, gridid_t gridid);
   bool objectgrid_change(structure::Object *object, 
                          gridid_t new_gridid, 
                          gridid_t old_gridid);

   //人数判断
   bool canenter();
   bool isfull();

   //进入检查 -1 异常 0 不可以进入 1 可以进入 2 启动备份场景
   int8_t entercheck(int16_t connectionid);
   void get_vrect_inradius(structure::vrect_t *vrect, 
                           int32_t radius, 
                           gridid_t gridid);

   //数据初始化相关
   inline Grid *getgrid(gridid_t id);
   inline void setload_map(const char *filename);
   inline void setload_monster(const char *filename);
   inline void setload_region(const char *filename);
   inline void setfuben_sourcescene_id(const sceneid_t sceneid);
   inline void setfuben_teamleader(const guid_t guid);
   inline void setfuben_nouser_closetime(const uint32_t time);
   inline void setfuben_timer(const uint32_t timer);
   inline void setfuben_param(const uint32_t index, const int32_t value);
   inline sceneid_t getfuben_sourcescene_id();
   inline guid_t getfuben_teamleader();
   inline uint32_t getfuben_nouser_closetime();
   inline uint32_t getfuben_timer();
   inline int32_t getfuben_param(const uint32_t index);

 public: //副本相关
   inline void fuben_setdata(fuben_data_t *fuben_data);
   
 public:
   inline int32_t get_rand100();
   inline int32_t get_rand10000();

 public:
   inline char *get_map_filename();
   inline void settype(uint8_t type);
   inline uint8_t gettype() const;
   inline void set_safelevel(uint8_t level);
   inline uint8_t get_safelevel() const;
   inline void set_restrictivemode(uint8_t mode);
   inline uint8_t get_restrictivemode() const;
   inline void setpunish(bool punish);
   inline bool getpunish() const;
   
 public: //timer
   inline void timerstart(uint32_t destroytime, uint32_t nowtime);
   inline void timerstop();

 public: //mail
   void mail_normal(structure::Object *player, 
                    const char *receivername, 
                    const char *content);
   void mail_system(const char *receivername, const char *content);
   void mail_script(const char *receivername, 
                    int32_t param0, 
                    int32_t param1 = -1, 
                    int32_t param2 = -1, 
                    int32_t param3 = -1);

 public: //地图的一些属性
   inline int32_t get_clientres() const;
   inline structure::position_t *get_canenter_position();
   inline void set_canenter_position(int32_t x, int32_t z);
   inline uint32_t get_figtttime() const;
   inline void set_fighttime(uint32_t time);
   inline void setthread(Thread *thread);
   inline Thread *getthread();
   inline void setlock(bool lock);
   inline void incmoney(int32_t count);
   inline void decmoney(int32_t count);
   inline int32_t getmoney() const;
   inline void set_broadcast_radius(int32_t radius);
   inline int32_t get_broadcast_radius() const;
   virtual bool remove_allobject();

 protected:
   bool isinit_;
   sceneid_t id_; //场景ID
   uint8_t status_; //状态
   uint8_t type_; //类型
   Map *map_; //地图数据
   Grid *grid_; //格子*
   gridinfo_t gridinfo_; //格子信息
   scene::manager::Net *netmanager_; //网络管理器
   scene::manager::RecycleConnection *recycle_connectionmanager_; //网络连接回收器
   structure::manager::Object *objectmanager_; //对象管理器
   structure::manager::ObjectSingle *humanmanager_; //玩家管理器
   structure::manager::ObjectSingle *monstermanager_; //怪物管理器
   scene::manager::Region *regionmanager_; //区域管理器
   scene::manager::TimerDoing *timerdoing_manager_; //定时动作器
   scene::manager::PKRegion *pkregion_manager_; //安全区域管理器
   scene::manager::Variable *variable_manager_; //动态数据管理器
   scene::manager::DropPosition *dropposition_manager_; //掉落管理器
   scene::manager::Scene *scenemanager_; //场景管理器
   Timer *timers_; //定时器
   ChatPipeline *chatpipeline_; //聊天缓冲器
   pf_base::TinyTimer quittimer_; //退出的定时器（迷你）
   pf_base::TinyTimer timer_; //场景定时器（迷你）
   uint8_t safelevel_; //安全等级
   uint8_t restrictivemode_; //限制模式
   bool punish_; //是否有惩罚
   pf_base::TinyTimer fuben_quittimer_; //副本退出定时器
   structure::position_t canenter_position_; //可以进入的点
   uint32_t broadcast_radius_; //广播的范围
   pf_base::hashmap::Template<int32_t, bool> config_bool_; //布尔类型配置

 private:
   pf_util::RandomTable<100> randomtable100_;
   pf_util::RandomTable<10000> randomtable10000_;

};

}; //namespace scene

#include "scene/scene.inl"

#endif //SCENE_SCENE_H_
