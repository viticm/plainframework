#include "packet/sayhello.h"
#include "packet/helloworld.h"
#include "net.h"

//包ID是否有效
bool __stdcall is_valid_packetid(uint16_t id) {
  return kPacketIdMin <= id && kPacketIdMax >= id;
}

bool __stdcall is_valid_dynamic_packet_id(uint16_t id) {
  return 20000 <= id && 30000 >=id;
}

//创建包生产器
bool __stdcall registerfactories() {
  NET_PACKET_FACTORYMANAGER_POINTER->add_factory(new SayHelloFactory());
  NET_PACKET_FACTORYMANAGER_POINTER->add_factory(new HelloWorldFactory());
  return true;
}

void init_net_packets() {
  if (is_null(NET_PACKET_FACTORYMANAGER_POINTER)) {
    std::unique_ptr<pf_net::packet::FactoryManager> 
      temp{new pf_net::packet::FactoryManager()};
    g_packetfactory_manager = std::move(temp);
    if (is_null(NET_PACKET_FACTORYMANAGER_POINTER)) return;
  }
  NET_PACKET_FACTORYMANAGER_POINTER->set_size(kPacketIdMax - kPacketIdMin + 1);
  NET_PACKET_FACTORYMANAGER_POINTER
    ->set_function_register_factories(registerfactories);
  NET_PACKET_FACTORYMANAGER_POINTER
    ->set_function_is_valid_packet_id(is_valid_packetid);
  NET_PACKET_FACTORYMANAGER_POINTER
    ->set_function_is_valid_dynamic_packet_id(is_valid_dynamic_packet_id);
}
