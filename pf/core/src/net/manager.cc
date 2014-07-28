#include "pf/base/log.h"
#include "pf/base/time_manager.h"
#include "pf/base/util.h"
#include "pf/net/packet/factorymanager.h"
#include "pf/performance/eyes.h"
#include "pf/net/manager.h"

bool g_net_stream_usepacket = true;

namespace pf_net {

Manager::Manager() {
  __ENTER_FUNCTION
    setactive(true);
  __LEAVE_FUNCTION
}

Manager::~Manager() {
  //do nothing
}

bool Manager::heartbeat() {
  __ENTER_FUNCTION
    uint32_t currenttime = g_time_manager->get_current_time();
    uint16_t connectioncount = getcount();
    uint16_t i;
    for (i = 0; i < connectioncount; ++i) {
      if (ID_INVALID == connection_idset_[i]) continue;
      connection::Base* connection = NULL;
      connection = pool_.get(connection_idset_[i]);
      if (NULL == connection) {
        Assert(false);
        return false;
      }
      if (!connection->heartbeat(currenttime)) {
        remove(connection);
        Assert(false);
      }
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

void Manager::loop() {
  __ENTER_FUNCTION
    while (isactive()) {
      bool result = false;
      try {
        result = select();
        Assert(result);
        result = processexception();
        Assert(result);
        result = processinput();
        Assert(result);
        result = processoutput();
        Assert(result); 
        if (PERFORMANCE_EYES_POINTER) { //网络性能监视
          uint16_t connectioncount = getcount();
          PERFORMANCE_EYES_POINTER->set_onlinecount(connectioncount);
          PERFORMANCE_EYES_POINTER->set_connectioncount(connectioncount);
          uint64_t sendbytes = get_send_bytes();
          uint64_t receivebytes = get_receive_bytes();
          PERFORMANCE_EYES_POINTER->set_sendbytes(sendbytes);
          PERFORMANCE_EYES_POINTER->set_receivebytes(receivebytes);
        }
      } catch(...) {
        
      }
      try {
        result = processcommand();
        Assert(result);
        //ERRORPRINTF("processcommand");
      } catch(...) {
        
      }

      try {
        result = heartbeat();
        Assert(result);
      } catch(...) {

      }
    }
  __LEAVE_FUNCTION
}

bool Manager::isactive() {
  return active_;
}

void Manager::setactive(bool active) {
  active_ = active;
}

void Manager::broadcast(packet::Base* packet) {
  __ENTER_FUNCTION
    uint16_t connectioncount = getcount();
    uint16_t i;
    for (i = 0; i < connectioncount; ++i) {
      if (ID_INVALID == connection_idset_[i]) continue;
      connection::Base* connection = NULL;
      connection = pool_.get(connection_idset_[i]);
      if (NULL == connection) {
        Assert(false); 
        continue;
      }
      connection->sendpacket(packet);
    }
  __LEAVE_FUNCTION
}

} //namespace pf_net
