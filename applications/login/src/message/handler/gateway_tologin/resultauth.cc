#include "common/net/packet/gateway_tologin/resultauth.h"

using namespace common::net::packet::gateway_tologin;


uint32_t ResultAuthHandler::execute(ResultAuth *packet,
                                    pf_net::connection::Base *connection) {
  __ENTER_FUNCTION
    return kPacketExecuteStatusContinue;
  __LEAVE_FUNCTION
    return kPacketExecuteStatusError;
}
