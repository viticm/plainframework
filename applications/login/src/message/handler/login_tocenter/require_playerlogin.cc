#include "common/net/packet/login_tocenter/require_playerlogin.h"

using namespace common::net::packet::gateway_tocenter;

uint32_t RequirePlayerLoginHandler::execute(
    RequirePlayerLogin *packet,
    pf_net::connection::Base *connection) {
  __ENTER_FUNCTION
    return kPacketExecuteStatusContinue;
  __LEAVE_FUNCTION
    return kPacketExecuteStatusError;
}
