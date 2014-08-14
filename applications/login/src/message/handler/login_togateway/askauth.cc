#include "common/net/packet/login_togateway/askauth.h"

using namespace common::net::packet::login_togateway;

uint32_t AskAuthHandler::execute(
    AskAuth *packet,
    pf_net::connection::Base *connection) {
  __ENTER_FUNCTION
    return kPacketExecuteStatusContinue;
  __LEAVE_FUNCTION
    return kPacketExecuteStatusError;
}
