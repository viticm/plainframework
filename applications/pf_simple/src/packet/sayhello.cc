#include "packet/helloworld.h"
#include "packet/sayhello.h"

bool SayHello::read(pf_net::stream::Input &istream) {
  istream.read_string(str_, sizeof(str_) - 1);
  return true;
}

bool SayHello::write(pf_net::stream::Output &ostream) {
  ostream << str_;
  return true;
}

uint32_t SayHello::execute(pf_net::connection::Basic *connection) {
  return SayHelloHandler::execute(this, connection);
}

uint32_t SayHelloHandler::execute(
    SayHello *packet, pf_net::connection::Basic *connection) {
  pf_basic::io_cwarn("------------------------net-------------------------");
  pf_basic::io_cdebug("SayHelloHandler recv: %s", packet->get_str());
  HelloWorld pkt;
  pkt.set_str("hello world!");
  connection->send(&pkt);
  return kPacketExecuteStatusContinue;
}
