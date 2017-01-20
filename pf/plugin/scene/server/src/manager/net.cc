#include "scene/chat_pipeline.h"
#include "scene/manager/net.h"

namespace scene {

namespace manager {

Net::Net() {
  sceneid_ = ID_INVALID;
}

Net::~Net() {
  //do nothing
}

sceneid_t Net::get_sceneid() const {
  return sceneid_;
}

void Net::set_sceneid(sceneid_t id) {
  sceneid_ = id;
}

bool Net::process_cachecommand() {
  return true;
}

bool Net::sendpacket(pf_net::packet::Base *packet,
                     int16_t serverid,
                     uint32_t flag) {
  USE_PARAM(packet);
  USE_PARAM(serverid);
  USE_PARAM(flag);
  return true;
}

void Net::broadcast(pf_net::packet::Base *packet, 
                    scene::ChatPipeline *chatpipeline, 
                    int32_t status) {
  __ENTER_FUNCTION
    uint16_t connectioncount = getcount();
    uint16_t i;
    for (i = 0; i < connectioncount; ++i) { 
      if (ID_INVALID == connection_idset_[i]) continue;
      pf_net::connection::Base* connection = NULL;
      connection = pool_->get(connection_idset_[i]);
      if (NULL == connection) {
        Assert(false);
        continue;
      }
      int32_t _status = static_cast<int32_t>(connection->getstatus());
      if (status != -1 && status != _status) continue;
      connection->sendpacket(packet);
      if (chatpipeline) chatpipeline->dec_validcount();
    }
  __LEAVE_FUNCTION
}


} //namespace manager

} //namespace scene
