#include "pf/base/string.h"
#include "common/define/net/packet/id/servercenter.h"
#include "common/define/enum.h"
#include "common/net/packet/servercenter/script_execute.h"

using namespace pf_net::socket;
using namespace common::net::packet::servercenter;

ScriptExecute::ScriptExecute() {
  __ENTER_FUNCTION
    memset(function_, 0, sizeof(function_));
  __LEAVE_FUNCTION
}

bool ScriptExecute::read(InputStream &inputstream) {
  __ENTER_FUNCTION
    inputstream.read_string(function_, sizeof(function_));
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool ScriptExecute::write(OutputStream &outputstream) const {
  __ENTER_FUNCTION
    outputstream.write_string(function_);
    return true;
  __LEAVE_FUNCTION
    return false;
}

uint32_t ScriptExecute::execute(
    pf_net::connection::Base* connection) {
  __ENTER_FUNCTION
    uint32_t result = 0;
    result = ScriptExecuteHandler::execute(this, connection);
    return result;
  __LEAVE_FUNCTION
    return 0;
}

uint16_t ScriptExecute::getid() const {
  using namespace 
    common::define::net::packet::id::servercenter; 
  return kScriptExecute;
}

uint32_t ScriptExecute::getsize() const {
  uint32_t result = strlen(function_);
  return result;
}

const char *ScriptExecute::get_function() {
  return function_;
}

void ScriptExecute::set_function(const char *function) {
  pf_base::string::safecopy(function_, function, sizeof(function_));
}

pf_net::packet::Base *ScriptExecuteFactory::createpacket() {
  __ENTER_FUNCTION
    pf_net::packet::Base *result = new ScriptExecute();
    return result;
  __LEAVE_FUNCTION
    return NULL;
}

uint16_t ScriptExecuteFactory::get_packetid() const {
  using namespace 
    common::define::net::packet::id::servercenter; 
  return kScriptExecute;
}

uint32_t ScriptExecuteFactory::get_packet_maxsize() const {
  uint32_t result = sizeof(char) * 2048;
  return result;
}
