#include "message/delegator/account.h"
#include "pf/application/net/packet/gateway_tologin/resultauth.h"

using namespace pf_application::net::packet::gateway_tologin;

uint32_t ResultAuthHandler::execute(ResultAuth *packet, 
                                    pf_net::connection::Base *connection) {
  __ENTER_FUNCTION
    USE_PARAM(packet);
    USE_PARAM(connection);
    return kPacketExecuteStatusContinue;
  __LEAVE_FUNCTION
    return kPacketExecuteStatusError;
}
