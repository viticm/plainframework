#include "pf/net/packet/factorymanager.h"
#include "pf/application/define/net/packet/id/all.h"

/* packets { */
#include "pf/application/net/packet/serverserver/connect.h"
#include "pf/application/net/packet/gateway_tologin/resultauth.h"
#include "pf/application/net/packet/login_togateway/askauth.h"
/* } packets */

#include "pf/application/extend/net_packet_factorymanager.h"

namespace pf_net {

namespace packet {

void FactoryManager::extend_forconstruct() {
  __ENTER_FUNCTION
    using namespace pf_application::define::net::packet::id;
    //between server and server
#if defined(_PF_NET_GATEWAY) || \
    defined(_PF_NET_LOGIN) || \
    defined(_PF_NET_CENTER) || \
    defined(_PF_NET_SERVER)
    size_ = serverserver::kLast - serverserver::kFirst; //common for server
#endif

    //between gateway and login
#if defined(_PF_NET_GATEWAY) || defined(_PF_NET_LOGIN)
    size_ += gatewaylogin::kLast - gatewaylogin::kFirst - 1;
    size_ += gateway_tologin::kLast - gateway_tologin::kFirst - 1;
    size_ += login_togateway::kLast - login_togateway::kFirst - 1;
#endif
    //between login and client
#if defined(_PF_NET_LOGIN) || defined(_PF_NET_CLIENT)
    size_ += clientlogin::kLast - clientlogin::kFirst - 1;
    size_ += client_tologin::kLast - client_tologin::kFirst - 1;
    size_ += login_toclient::kLast - login_toclient::kFirst - 1;
#endif
    //between login and center
#if defined(_PF_NET_LOGIN) || defined(_PF_NET_CENTER)
    size_ += logincenter::kLast - logincenter::kFirst - 1;
    size_ += login_tocenter::kLast - login_tocenter::kFirst - 1;
    size_ += center_tologin::kLast - center_tologin::kFirst - 1;
#endif
    //between server and center
#if defined(_PF_NET_SERVER) || defined(_PF_NET_CENTER)
    size_ += servercenter::kLast - servercenter::kFirst - 1;
    size_ += server_tocenter::kLast - server_tocenter::kFirst - 1;
    size_ += center_toserver::kLast - center_toserver::kFirst - 1;
#endif
    //between client and server
#if defined(_PF_NET_CLIENT) || defined(_PF_NET_SERVER)
    size_ += clientserver::kLast - clientserver::kFirst - 1;
    size_ += client_toserver::kLast - client_toserver::kFirst - 1;
    size_ += server_tocenter::kLast - server_tocenter::kFirst - 1;
#endif
  __LEAVE_FUNCTION
}

bool FactoryManager::extend_for_packetid_isvalid(uint16_t id) {
  __ENTER_FUNCTION
    bool result = true;
    using namespace pf_application::define::net::packet::id;
#if defined(_PF_NET_GATEWAY) /* { */
    result = (serverserver::kFirst < id && id < serverserver::kLast) ||  
             (gatewaylogin::kFirst < id && id < gatewaylogin::kLast) ||
             (gateway_tologin::kFirst < id && id < gateway_tologin::kLast) || 
             (login_togateway::kFirst < id && id < login_togateway::kLast);
#elif defined(_PF_NET_LOGIN) /* }{ */
    result = (serverserver::kFirst < id && id < serverserver::kLast) ||
             (gatewaylogin::kFirst < id && id < gatewaylogin::kLast) ||
             (gateway_tologin::kFirst < id && id < gateway_tologin::kLast) ||
             (login_togateway::kFirst < id && id < login_togateway::kLast)
             (clientlogin::kFirst < id && id < clientlogin::kLast) ||
             (client_tologin::kFirst < id && id < client_tologin::kLast) ||
             (login_tocenter::kFirst < id && id < login_toclient::kLast) ||
             (logincenter::kFirst < id && id < logincenter::kLast) ||
             (login_tocenter::kFirst < id && id < login_tocenter::kLast) || 
             (center_tologin::kFirst < id && id < center_tologin::kLast);
#elif defined(_PF_NET_CENTER) /* }{ */
    result = (serverserver::kFirst < id && id < serverserver::kLast) ||
             (logincenter::kFirst < id && id < logincenter::kLast) ||
             (login_tocenter::kFirst < id && id < login_tocenter::kLast) ||
             (center_tologin::kFirst < id && id < center_tologin::kLast) ||
             (servercenter::kFirst < id && id < servercenter::kLast) ||
             (server_tocenter::kFirst < id && id < server_tocenter::kLast) ||
             (center_toserver::kFirst < id && id < center_toserver::kLast);
#elif defined(_PF_NET_SERVER) /* }{ */
    result = (serverserver::kFirst < id && id < serverserver::kLast) ||
             (clientserver::kFirst < id && id < clientserver::kLast) ||
             (client_toserver::kFirst < id && id < client_toserver::kLast) ||
             (server_toclient::kFirst < id && id < server_toclient::kLast) ||
             (servercenter::kFirst < id && id < servercenter::kLast) ||
             (server_tocenter::kFirst < id && id < server_tocenter::kLast) ||
             (center_toserver::kFirst < id && id < center_toserver::kLast);
#elif defined(_PF_NET_CLIENT) /* }{ */
    result = (clientlogin::kFirst < id && id < clientlogin::kLast) ||
             (login_tocenter::kFirst < id && id < login_toclient::kLast) ||
             (login_tocenter::kFirst < id && id < login_toclient::kLast) ||
             (clientserver::kFirst < id && id < clientserver::kLast) || 
             (client_toserver::kFirst < id && id < client_toserver::kLast) ||
             (server_toclient::kFirst < id && id < server_toclient::kLast);
#endif /* } */
    return result;
  __LEAVE_FUNCTION
    return false;
}

void FactoryManager::extend_forinit() {
  __ENTER_FUNCTION
    using namespace pf_application::net::packet;

#if defined(_PF_NET_GATEWAY) || \
  defined(_PF_NET_LOGIN) || \
  defined(_PF_NET_CENTER) || \
  defined(_PF_NET_SERVER)
  addfactory(new serverserver::ConnectFactory());
#endif

#if defined(_PF_NET_GATEWAY) || defined(_PF_NET_LOGIN) /* { */
    addfactory(new gateway_tologin::ResultAuthFactory());
    addfactory(new login_togateway::AskAuthFactory());
#endif /* } */

#if defined(_PF_NET_LOGIN) || defined(_PF_NET_CLIENT) /* { */

#endif /* } */

#if defined(_PF_NET_LOGIN) || defined(_PAP_NET_CENTER) /* { */

#endif /* } */

#if defined(_PF_NET_SERVER) || defined(_PF_NET_CENTER) /* { */

#endif /* } */

#if defined(_PF_NET_CLIENT) || defined(_PF_NET_SERVER) /* { */

#endif /* } */
  __LEAVE_FUNCTION
}

} //namespace packet

} //namespace pf_net
