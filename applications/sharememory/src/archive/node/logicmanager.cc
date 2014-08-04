#include "common/setting.h"
#include "archive/data/globaldata.h"
#include "archive/node/logic.h"
#include "archive/node/logicmanager.h"

int32_t g_cmd_model = 0;

using namespace archive::node;

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
      type_t type = data[i].type;
      switch (type) {
        case kTypeGlobal: {
          node = new Logic<globaldata_t>;
          node->setpool(new pf_sys::memory::share::UnitPool<globaldata_t>);
          node->setdata(data[i]);
          node->settype(type);
          break;
        }
        default:
          break;
      }
      setnode(i, node);
      createlog(data[i].key);
    }
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
      type_t type = data[i].type;
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
      type_t type = data[i].type;
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
      type_t type = data[i].type;
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
