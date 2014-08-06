#include "pf/net/connection/server.h"

namespace pf_net {

namespace connection {

Server::Server() {
  status_ = 0;
}

Server::~Server() {
  //do nothing
}

bool Server::init() {
  __ENTER_FUNCTION
    Base::setdisconnect(false);
    Base::resetkick();
    bool result = Base::init();
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Server::processinput() {
  __ENTER_FUNCTION
    bool result = false;
    result = Base::processinput();
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Server::processoutput() {
  __ENTER_FUNCTION
    bool result = false;
    result = Base::processoutput();
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Server::processcommand(bool option) {
  __ENTER_FUNCTION
    bool result = false;
    result = Base::processcommand(option);
    return result;
  __LEAVE_FUNCTION
    return false;
}

void Server::cleanup() {
  __ENTER_FUNCTION
    Base::cleanup();
  __LEAVE_FUNCTION
}

bool Server::heartbeat(uint32_t time) {
  __ENTER_FUNCTION
    bool reslut = false;
    reslut = Base::heartbeat(time);
    return reslut;
  __LEAVE_FUNCTION
    return false;
}

bool Server::isserver() {
  return true;
}

bool Server::isplayer() {
  return false;
}

bool Server::isbilling() {
  return false;
}

bool Server::islogin() {
  return false;
}

bool Server::isvalid() {
  __ENTER_FUNCTION
    bool result = false;
    result = Base::isvalid();
    return result;
  __LEAVE_FUNCTION
    return false;
}

bool Server::sendpacket(packet::Base *packet) {
  __ENTER_FUNCTION
    bool result = false;
    result = Base::sendpacket(packet);
    return result;
  __LEAVE_FUNCTION
    return false;
}

} //namespace connection

} //namespace pap_server_common_net
