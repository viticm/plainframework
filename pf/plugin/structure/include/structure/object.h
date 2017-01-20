/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id object.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm@126.com>/viticm.ti@gmail.com
 * @date 2015/03/17 15:30
 * @uses structure object classes
 *       cn: 服务器对象相关
 *           该类的设计类似于网络连接的设计模式，不过针对性更强
 *           这是个接口类，大部分逻辑需要在子类中实现才可使用
**/
#ifndef STRUCTURE_OBJECT_H_
#define STRUCTURE_OBJECT_H_

#include "structure/config.h"
#include "structure/define.h"

namespace structure {

class PF_PLUGIN_API Object {

 public:
   Object();
   virtual ~Object();

 public:
   virtual bool init(object_t *object);
   virtual void clear();
   virtual bool heartbeat(uint32_t time = 0); //激活时执行的逻辑
   virtual bool heartbeat_outscene(uint32_t time = 0); //未激活时执行的逻辑
   virtual void update_viewcharacter(int32_t old_viewgroup);

 public: //event
   virtual void on_enterscene(); //进入场景
   virtual void on_leavescene(); //离开场景
   virtual void on_enterscene_success(); //进入场景成功
   virtual void on_register_togrid(); //注册到地图格子上
   virtual void on_unregister_fromgrid(); //从地图格子上撤销注册

 public:
   int8_t gettype() const;
   objectid_t getid() const;
   void setid(objectid_t id);
   virtual int64_t get_uniqueid() const;
   virtual const char *getname() = 0;
   void set_poolid(uint32_t id);
   uint32_t get_poolid() const;
   void set_singlemanager_index(uint32_t index);
   uint32_t get_singlemanager_index() const;
   uint32_t get_logictime() const;
   uint32_t get_lasttime() const;
   uint32_t get_nowtime() const;
   uint32_t get_createtime() const;
   virtual void set_viewgroup(int32_t viewgroup);
   virtual int32_t get_viewgroup() const;
   virtual Object *getspecific_object_insame_scene_byid(objectid_t id) = 0;
   virtual Object *getspecific_human_insame_scene_byguid(guid_t id) = 0;
   objectlist_node_t *get_objectnode();
   void setguid(guid_t guid);
   guid_t getguid() const;
   void set_dataid(uint32_t id);
   uint32_t get_dataid() const;

 public: //可视相关
   virtual bool can_viewme(const Object *object);
   virtual bool can_viewhim(const Object *object);
   virtual bool can_prev_viewme(const Object *object, int32_t old_viewgroup);
   virtual bool can_prev_viewhim(const Object *object, int32_t old_viewgroup);
   virtual bool can_stealth_viewme(const Object *object);
   virtual bool can_stealth_viewhim(const Object *object);

 public:
   bool isactive() const;
   virtual void setactive(bool active, Object *object) = 0;
   const position_t *getposition() const;
   void setposition(position_t *position);
   virtual void teleport(const position_t *position, 
                         bool show_changestate = true) = 0;
   void setdirection(float direction);
   float getdirection() const;
   void setscene(void *scene);
   void *getscene();
   void set_gridid(gridid_t id);
   gridid_t get_gridid() const;
   virtual void set_scriptid(scriptid_t id);
   virtual scriptid_t get_scriptid() const;
   virtual objectid_t get_lockedtarget() const;
   void *get_aicontroller();
   void set_aicontroller(void *aicontroller);
   aidata_t *get_aidata();
   void set_aidata(aidata_t *data);

 public: //范围
   bool radius_invalid(float x1, float z1, float x2, float z2, float radius);
   bool radius_invalid(const Object *object, float radius);
   bool radius_invalid(const position_t *position, float radius);

 public:
   virtual bool in_pkarea() const;
   virtual const campdata_t *get_campdata();

 public:
   uint8_t get_logiccount() const;
   void add_logiccount();
   uint8_t get_second_logiccount() const;
   void add_second_logiccount();
   int32_t get_move_logiccount() const;
   void add_move_logiccount();

 public: //网络接口
   virtual pf_net::packet::Base *create_newobject_packet() = 0;
   virtual void destroy_newobject_packet(pf_net::packet::Base *packet) = 0;
   virtual pf_net::packet::Base *create_deleteobject_packet() = 0;
   virtual void destroy_deleteobject_packet(pf_net::packet::Base *packet) = 0;
   //这个接口相当于一个事件，放在需要处理新建对象网络包的地方去
   virtual void process_newobject_packet(pf_net::packet::Base *packet, 
                                         Object *object) = 0;
   virtual pf_net::connection::Base *get_netconnection();
   virtual void process_talk(); //处理谈话，AI内容？

 public:
   virtual bool updategrid();

 protected:
   int8_t type_;
   objectid_t id_;
   uint32_t poolid_;
   uint32_t singlemanager_index_; //单管理器的索引，参考成吉思汗OL，暂时加上
   int32_t viewgroup_;
   bool isactive_;
   uint8_t logiccount_;
   uint8_t second_logiccount_;
   int32_t move_logiccount_;
   position_t position_;
   pf_net::connection::Base *netconnection_; //网络连接指针
   objectid_t lockedtarget_; //锁定的对象ID
   guid_t guid_; //唯一ID
   uint32_t dataid_; //数据ID，一般为配置

 protected:
   void set_move_logiccount(int32_t count);

 private:
   objectlist_node_t *listnode_;
   float direction_;
   uint32_t nowtime_; //当前时间
   uint32_t lasttime_; //上次逻辑运行完成后的时间
   uint32_t createtime_; //对象的创建时间
   gridid_t gridid_; //格子ID
   void *scene_; //场景指针
   void *aicontroller_; //AI控制器指针
   aidata_t *aidata_;

 private:
   void updatetime(uint32_t time);

};

}; //namespace structure

#endif //STRUCTURE_OBJECT_H_
