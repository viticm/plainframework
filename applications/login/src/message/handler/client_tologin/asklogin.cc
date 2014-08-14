#include "common/net/packet/client_tologin/asklogin.h"

using namespace common::net::packet::client_tologin;

uint32_t AskLoginHandler::execute(AskLogin *packet,
                                  pf_net::connection::Base *connection) {
  __ENTER_FUNCTION
    return kPacketExecuteStatusContinue;
  __LEAVE_FUNCTION
    return kPacketExecuteStatusError;
}
