#include "common/net/packet/login_toclient/turnstatus.h"

using namespace common::net::packet::login_toclient;

uint32_t TurnStatusHandler::execute(
    TurnStatus *packet,
    pf_net::connection::Base *connection) {
  __ENTER_FUNCTION
    return kPacketExecuteStatusContinue;
  __LEAVE_FUNCTION
    return kPacketExecuteStatusError;
}
