#include "common/net/packet/servercenter/script_execute.h"

using namespace common::net::packet::servercenter;
uint32_t ScriptExecuteHandler::execute(ScriptExecute* packet, 
                                       pf_net::connection::Base* connection) {
  __ENTER_FUNCTION
    return kPacketExecuteStatusContinue;
  __LEAVE_FUNCTION
    return kPacketExecuteStatusError;
}
