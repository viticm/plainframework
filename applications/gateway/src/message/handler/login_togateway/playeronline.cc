#include "common/net/packet/login_togateway/playeronline.h"

using namespace common::net::packet::login_togateway;

uint32_t PlayerOnlineHandler::execute(
    PlayerOnline *packet,
    pf_net::connection::Base *connection) {
  __ENTER_FUNCTION
    USE_PARAM(connection);
    FAST_LOG(kApplicationLogFile,
             "[gateway][message][handler][login_togateway]"
             " (PlayerOnlineHandler::execute) success."
             " online player: %d",
             packet->getonline());
    return kPacketExecuteStatusContinue;
  __LEAVE_FUNCTION
    return kPacketExecuteStatusError;
}
