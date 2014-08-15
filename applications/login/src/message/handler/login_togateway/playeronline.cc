#include "common/net/packet/login_togateway/playeronline.h"

using namespace common::net::packet::login_togateway;

uint32_t PlayerOnlineHandler::execute(
    PlayerOnline *packet,
    pf_net::connection::Base *connection) {
  __ENTER_FUNCTION
    return kPacketExecuteStatusContinue;
  __LEAVE_FUNCTION
    return kPacketExecuteStatusError;
}
