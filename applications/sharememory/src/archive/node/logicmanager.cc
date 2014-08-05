#include "common/setting.h"
#include "archive/data/globaldata.h"
#include "archive/node/logic.h"
#include "archive/node/logicmanager.h"

archive::node::LogicManager *g_archive_nodelogic_manager = NULL;

template <>
archive::node::LogicManager 
  *pf_base::Singleton<archive::node::LogicManager>::singleton_ = NULL;

using namespace archive::node;

LogicManager::LogicManager() {
  memset(logicdata_, 0, sizeof(logicdata_));
}

LogicManager *LogicManager::getsingleton_pointer() {
  return singleton_;
}

LogicManager &LogicManager::getsingleton() {
  Assert(singleton_);
  return *singleton_;
}

void LogicManager::clearlog() {
  __ENTER_FUNCTION
#if __LINUX__
    FILE *fp = fopen("./clearsmu.sh", "w");
    if (fp) {
      char message[256] = {0};
      fwrite(message, 1, strlen(message), fp);
      fclose(fp);
    }
#endif
  __LEAVE_FUNCTION
}

void LogicManager::createlog(uint32_t key) {
  __ENTER_FUNCTION
    USE_PARAM(key);
#if __LINUX__
    FILE *fp = fopen("./clearsmu.sh", "a+");
    if (fp) {
      char message[256] = {0};
      snprintf(message, sizeof(message) - 1, "ipcrm -M %u\n", key);
      fwrite(message, 1, strlen(message), fp);
      fclose(fp);
    }
#endif
  __LEAVE_FUNCTION
}

void LogicManager::setnode(int32_t index, void *node) {
  __ENTER_FUNCTION
    Assert(index >= 0 && index <= ARCHIVE_OBJMAX);
    logicdata_[index] = node;
  __LEAVE_FUNCTION
}

void *LogicManager::getnode(int32_t index) {
  __ENTER_FUNCTION
    Assert(index >= 0 && index <= ARCHIVE_OBJMAX);
    return logicdata_[index];
  __LEAVE_FUNCTION
    return NULL;
}

bool LogicManager::allocate() {
  __ENTER_FUNCTION
    using namespace common::sharememory;
    clearlog();
    uint16_t obj_count = SETTING_POINTER->share_memory_info_.obj_count;
    common::share_memory_data_t *data = 
      SETTING_POINTER->share_memory_info_.data;
    uint16_t i = 0;
    for (i = 0; i < obj_count; ++i) {
      void *node = NULL;
      type_t type = static_cast<type_t>(data[i].type);
      switch (type) {
        case kTypeGlobal: {
          node = new Logic<globaldata_t>;
          (reinterpret_cast<Logic<globaldata_t> *>(node))
            ->setpool(new pf_sys::memory::share::UnitPool<globaldata_t>);
          (reinterpret_cast<Logic<globaldata_t> *>(node))
            ->setdata(data[i]);
          (reinterpret_cast<Logic<globaldata_t> *>(node))
            ->settype(type);
          break;
        }
        default:
          break;
      }
      setnode(i, node);
      createlog(data[i].key);
    }
    SLOW_LOG(APPLICATION_NAME, 
             "[archive.node] (LogicManager::allocate) success");
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool LogicManager::init() {
  __ENTER_FUNCTION
    using namespace common::sharememory;
    uint16_t obj_count = SETTING_POINTER->share_memory_info_.obj_count;
    common::share_memory_data_t *data = 
      SETTING_POINTER->share_memory_info_.data;
    uint16_t i = 0;
    for (i = 0; i < obj_count; ++i) {
      type_t type = static_cast<type_t>(data[i].type);
      switch (type) {
        case kTypeGlobal: {
          Logic<globaldata_t>* node = 
            reinterpret_cast<Logic<globaldata_t>* >(getnode(i));
          Assert(node);
          node->init(1);
          break;
        }
        default:
          break;
      }
    }
    SLOW_LOG(APPLICATION_NAME, 
             "[archive.node] (LogicManager::init) success");
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool LogicManager::release() {
  __ENTER_FUNCTION
    using namespace common::sharememory;
    uint16_t obj_count = SETTING_POINTER->share_memory_info_.obj_count;
    common::share_memory_data_t *data = 
      SETTING_POINTER->share_memory_info_.data;
    uint16_t i = 0;
    for (i = 0; i < obj_count; ++i) {
      type_t type = static_cast<type_t>(data[i].type);
      switch (type) {
        case kTypeGlobal: {
          Logic<globaldata_t>* node = 
            reinterpret_cast<Logic<globaldata_t>* >(getnode(i));
          SAFE_DELETE(node);
          break;
        }
        default:
          break;
      }
    }
    SLOW_LOG(APPLICATION_NAME, 
             "[archive.node] (LogicManager::release) success");
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool LogicManager::tick() {
  __ENTER_FUNCTION
    using namespace common::sharememory;
    uint16_t obj_count = SETTING_POINTER->share_memory_info_.obj_count;
    common::share_memory_data_t *data = 
      SETTING_POINTER->share_memory_info_.data;
    uint16_t i = 0;
    for (i = 0; i < obj_count; ++i) {
      type_t type = static_cast<type_t>(data[i].type);
      switch (type) {
        case kTypeGlobal: {
          Logic<globaldata_t>* node = 
            reinterpret_cast<Logic<globaldata_t>* >(getnode(i));
          Assert(node);
          node->tick();
          break;
        }
        default:
          break;
      }
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}
