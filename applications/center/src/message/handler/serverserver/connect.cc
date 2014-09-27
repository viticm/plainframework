#include "pf/base/log.h"
#include "common/setting.h"
#include "common/define/enum.h"
#include "common/net/packet/serverserver/connect.h"

using namespace common::net::packet::serverserver;
uint32_t ConnectHandler::execute(Connect* packet, 
                                 pf_net::connection::Base* connection) {
  __ENTER_FUNCTION
    int16_t serverid = packet->get_serverid();
    FAST_LOG(kApplicationLogFile,
             "[message.handler.serverver]"
             " (ConnectHandler::execute) success"
             " server id: %d",
             serverid);
    common::server_data_t *serverdata = 
      SETTING_POINTER->get_serverinfo(serverid);
    if (0 == serverdata->type) {
      connection->setstatus(kConnectionStatusGameServerReady);
    }
    return kPacketExecuteStatusContinue;
  __LEAVE_FUNCTION
    return kPacketExecuteStatusError;
}
