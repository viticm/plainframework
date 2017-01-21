#include "packet/helloworld.h"

bool HelloWorld::read(pf_net::stream::Input &istream) {
  istream.read_string(str_, sizeof(str_) - 1);
  return true;
}

uint32_t HelloWorld::execute(pf_net::connection::Basic *connection) {
  return HelloWorldHandler::execute(this, connection);
}

bool HelloWorld::write(pf_net::stream::Output &ostream) {
  ostream << str_;
  return true;
}

uint32_t HelloWorldHandler::execute(
    HelloWorld *packet, pf_net::connection::Basic *connection) {
  pf_basic::io_cdebug("HelloWorldHandler recv: %s", packet->get_str());
  pf_basic::io_cwarn("------------------------net-------------------------");
  return kPacketExecuteStatusContinue;
}
