#include "engine/thread/net/incoming.h"

using namespace engine::thread::net;

Incoming::Incoming() {
  //do nothing
}

Incoming::~Incoming() {
  //do nothing
}

bool Incoming::init(uint16_t connectionmax,
                    uint16_t listenport,
                    const char *listenip) {
  __ENTER_FUNCTION
    isactive_ = 
      connection::manager::Incoming::init(connectionmax, listenport, listenip);
    return isactive_;
  __LEAVE_FUNCTION
    return false;
}

void Incoming::run() {
  __ENTER_FUNCTION
    if (isactive()) connection::manager::Incoming::loop();
  __LEAVE_FUNCTION
}

void Incoming::stop() {
  __ENTER_FUNCTION
    isactive_ = false;
    connection::manager::Incoming::setactive(false);
  __LEAVE_FUNCTION
}

void Incoming::quit() {
  //do nothing
}

bool Incoming::isactive() const {
  return isactive_;
}
