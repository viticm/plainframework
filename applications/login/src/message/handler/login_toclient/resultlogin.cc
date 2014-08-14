#include "common/net/packet/login_toclient/resultlogin.h"

using namespace common::net::packet::login_toclient;

uint32_t ResultLoginHandler::execute(
    ResultLogin *packet,
    pf_net::connection::Base *connection) {
  __ENTER_FUNCTION
    return kPacketExecuteStatusContinue;
  __LEAVE_FUNCTION
    return kPacketExecuteStatusError;
}
