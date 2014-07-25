#include "pf/base/log.h"
#include "pf/application/net/packet/serverserver/connect.h"

using namespace pf_application::net::packet::serverserver;
uint32_t ConnectHandler::execute(Connect* packet, 
                                 pf_net::connection::Base* connection) {
  __ENTER_FUNCTION
    USE_PARAM(connection);
    FAST_LOG(kGatewayLogFile,
             "[gateway][message][handler][serverver]"
             " (ConnectHandler::execut) success"
             " server id: %d",
             packet->get_serverid());
    return kPacketExecuteStatusContinue;
  __LEAVE_FUNCTION
    return kPacketExecuteStatusError;
}
