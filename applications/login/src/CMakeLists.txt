SET(TF_CURRENT_BINARY_PATH_BIN "../../bin")
ADD_DEFINITIONS(-D_LOGIN)
ADD_DEFINITIONS(-D__SERVER__)

IF(CMAKE_SYSTEM MATCHES Linux)
  ADD_DEFINITIONS(-D_REENTRANT)
  ADD_DEFINITIONS(-DDONT_TD_VOID)
  ADD_DEFINITIONS(-D_PF_NET_EPOLL)
ELSE(CMAKE_SYSTEM MATCHES Linux)
ENDIF(CMAKE_SYSTEM MATCHES Linux)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})
LINK_DIRECTORIES(
  "./" #run dir
  "../../../pf/core/dll" #linux
)

SET(EXECUTABLE_OUTPUT_PATH ${TF_CURRENT_BINARY_PATH_BIN})


INCLUDE_DIRECTORIES("../../../pf/core/include",
                    "../include",
                    "../../common/include",
                    "../../../pf/dependencies/iconv/include",
                    "../../../pf/dependencies/lua/src",
                    "/usr/local/unixODBC/include")

##update_begin




SET (SOURCEFILES_CHARACTER_LIST
	../src/character/setting.cc
)

SET (SOURCEFILES_ENGINE_THREAD_NET_LIST
	../src/engine/thread/net/incoming.cc
	../src/engine/thread/net/login.cc
	../src/engine/thread/net/server.cc
)

SET (SOURCEFILES_ENGINE_THREAD_LIST
)

SET (SOURCEFILES_ENGINE_LIST
	../src/engine/system.cc
)

SET (SOURCEFILES_CONNECTION_COUNTER_LIST
	../src/connection/counter/center.cc
)

SET (SOURCEFILES_CONNECTION_MANAGER_LIST
	../src/connection/manager/incoming.cc
	../src/connection/manager/login.cc
	../src/connection/manager/server.cc
)

SET (SOURCEFILES_CONNECTION_QUEUE_LIST
	../src/connection/queue/center.cc
	../src/connection/queue/turn.cc
)

SET (SOURCEFILES_CONNECTION_LIST
	../src/connection/login.cc
)

SET (SOURCEFILES_DB_LIST
	../src/db/character.cc
)

SET (SOURCEFILES_MESSAGE_DELEGATOR_LIST
	../src/message/delegator/auth.cc
)

SET (SOURCEFILES_MESSAGE_HANDLER_CLIENT_TOLOGIN_LIST
	../src/message/handler/client_tologin/asklogin.cc
)

SET (SOURCEFILES_MESSAGE_HANDLER_GATEWAY_TOLOGIN_LIST
	../src/message/handler/gateway_tologin/resultauth.cc
)

SET (SOURCEFILES_MESSAGE_HANDLER_LOGIN_TOCENTER_LIST
	../src/message/handler/login_tocenter/require_playerlogin.cc
)

SET (SOURCEFILES_MESSAGE_HANDLER_LOGIN_TOCLIENT_LIST
	../src/message/handler/login_toclient/resultlogin.cc
	../src/message/handler/login_toclient/turnstatus.cc
)

SET (SOURCEFILES_MESSAGE_HANDLER_LOGIN_TOGATEWAY_LIST
	../src/message/handler/login_togateway/askauth.cc
	../src/message/handler/login_togateway/playerleave.cc
	../src/message/handler/login_togateway/playeronline.cc
)

SET (SOURCEFILES_MESSAGE_HANDLER_SERVERSERVER_LIST
	../src/message/handler/serverserver/connect.cc
)

SET (SOURCEFILES_MESSAGE_HANDLER_LIST
)

SET (SOURCEFILES_MESSAGE_LIST
)

SET (SOURCEFILES_LIST
	../src/main.cc
)

SET (HEADERFILES_APPLICATION_LIST
	../include/application/config.h
)

SET (HEADERFILES_CHARACTER_LIST
	../include/character/config.h
	../include/character/setting.h
)

SET (HEADERFILES_ENGINE_THREAD_NET_LIST
	../include/engine/thread/net/config.h
	../include/engine/thread/net/incoming.h
	../include/engine/thread/net/login.h
	../include/engine/thread/net/server.h
)

SET (HEADERFILES_ENGINE_THREAD_LIST
	../include/engine/thread/config.h
)

SET (HEADERFILES_ENGINE_LIST
	../include/engine/config.h
	../include/engine/system.h
)

SET (HEADERFILES_CONNECTION_COUNTER_LIST
	../include/connection/counter/center.h
	../include/connection/counter/config.h
)

SET (HEADERFILES_CONNECTION_MANAGER_LIST
	../include/connection/manager/config.h
	../include/connection/manager/incoming.h
	../include/connection/manager/login.h
	../include/connection/manager/server.h
)

SET (HEADERFILES_CONNECTION_QUEUE_LIST
	../include/connection/queue/center.h
	../include/connection/queue/config.h
	../include/connection/queue/turn.h
)

SET (HEADERFILES_CONNECTION_LIST
	../include/connection/config.h
	../include/connection/login.h
)

SET (HEADERFILES_DB_LIST
	../include/db/character.h
	../include/db/config.h
)

SET (HEADERFILES_MESSAGE_DELEGATOR_LIST
	../include/message/delegator/auth.h
)

SET (HEADERFILES_MESSAGE_LIST
)

SET (HEADERFILES_LIST
	../include/main.h
)

SET (RESOURCEFILES_LIST
)

SET (COMMON_SRC_DB_LIST
	../../common/src/db/define.cc
)

SET (COMMON_SRC_NET_CONNECTION_LIST
	../../common/src/net/connection/server.cc
)

SET (COMMON_SRC_NET_MANAGER_LIST
	../../common/src/net/manager/server.cc
)

SET (COMMON_SRC_NET_PACKET_CLIENT_TOLOGIN_LIST
	../../common/src/net/packet/client_tologin/asklogin.cc
)

SET (COMMON_SRC_NET_PACKET_LOGIN_TOCLIENT_LIST
	../../common/src/net/packet/login_toclient/resultlogin.cc
	../../common/src/net/packet/login_toclient/turnstatus.cc
)

SET (COMMON_SRC_NET_PACKET_LOGIN_TOCENTER_LIST
	../../common/src/net/packet/login_tocenter/require_playerlogin.cc
)

SET (COMMON_SRC_NET_PACKET_CENTER_TOLOGIN_LIST
)

SET (COMMON_SRC_NET_PACKET_LOGIN_TOGATEWAY_LIST
	../../common/src/net/packet/login_togateway/askauth.cc
	../../common/src/net/packet/login_togateway/playerleave.cc
	../../common/src/net/packet/login_togateway/playeronline.cc
)

SET (COMMON_SRC_NET_PACKET_GATEWAY_TOLOGIN_LIST
	../../common/src/net/packet/gateway_tologin/resultauth.cc
)

SET (COMMON_SRC_NET_PACKET_SERVERSERVER_LIST
	../../common/src/net/packet/serverserver/connect.cc
)

SET (COMMON_SRC_NET_PACKET_LIST
)

SET (COMMON_SRC_NET_LIST
	../../common/src/net/packetfactory.cc
)

SET (COMMON_SRC_LIST
	../../common/src/setting.cc
)

SET (COMMON_INCLUDE_DEFINE_NET_PACKET_ID_LIST
	../../common/include/common/define/net/packet/id/all.h
	../../common/include/common/define/net/packet/id/clientlogin.h
	../../common/include/common/define/net/packet/id/clientserver.h
	../../common/include/common/define/net/packet/id/config.h
	../../common/include/common/define/net/packet/id/gatewaylogin.h
	../../common/include/common/define/net/packet/id/logincenter.h
	../../common/include/common/define/net/packet/id/servercenter.h
	../../common/include/common/define/net/packet/id/serverserver.h
)

SET (COMMON_INCLUDE_DEFINE_NET_PACKET_LIST
	../../common/include/common/define/net/packet/config.h
)

SET (COMMON_INCLUDE_DEFINE_NET_LIST
	../../common/include/common/define/net/config.h
)

SET (COMMON_INCLUDE_DEFINE_LIST
	../../common/include/common/define/all.h
	../../common/include/common/define/config.h
	../../common/include/common/define/enum.h
	../../common/include/common/define/file.h
	../../common/include/common/define/log.h
	../../common/include/common/define/macros.h
)

SET (COMMON_INCLUDE_DB_LIST
	../../common/include/common/db/config.h
	../../common/include/common/db/define.h
)

SET (COMMON_INCLUDE_NET_CONNECTION_LIST
	../../common/include/common/net/connection/config.h
	../../common/include/common/net/connection/server.h
)

SET (COMMON_INCLUDE_NET_MANAGER_LIST
	../../common/include/common/net/manager/config.h
	../../common/include/common/net/manager/server.h
)

SET (COMMON_INCLUDE_NET_PACKET_CLIENT_TOLOGIN_LIST
	../../common/include/common/net/packet/client_tologin/asklogin.h
	../../common/include/common/net/packet/client_tologin/config.h
)

SET (COMMON_INCLUDE_NET_PACKET_LOGIN_TOCLIENT_LIST
	../../common/include/common/net/packet/login_toclient/config.h
	../../common/include/common/net/packet/login_toclient/resultlogin.h
	../../common/include/common/net/packet/login_toclient/turnstatus.h
)

SET (COMMON_INCLUDE_NET_PACKET_LOGIN_TOCENTER_LIST
	../../common/include/common/net/packet/login_tocenter/config.h
	../../common/include/common/net/packet/login_tocenter/require_playerlogin.h
)

SET (COMMON_INCLUDE_NET_PACKET_CENTER_TOLOGIN_LIST
)

SET (COMMON_INCLUDE_NET_PACKET_LOGIN_TOGATEWAY_LIST
	../../common/include/common/net/packet/login_togateway/askauth.h
	../../common/include/common/net/packet/login_togateway/config.h
	../../common/include/common/net/packet/login_togateway/playerleave.h
	../../common/include/common/net/packet/login_togateway/playeronline.h
)

SET (COMMON_INCLUDE_NET_PACKET_GATEWAY_TOLOGIN_LIST
	../../common/include/common/net/packet/gateway_tologin/config.h
	../../common/include/common/net/packet/gateway_tologin/resultauth.h
)

SET (COMMON_INCLUDE_NET_PACKET_SERVERSERVER_LIST
	../../common/include/common/net/packet/serverserver/config.h
	../../common/include/common/net/packet/serverserver/connect.h
)

SET (COMMON_INCLUDE_NET_PACKET_LIST
	../../common/include/common/net/packet/config.h
)

SET (COMMON_INCLUDE_NET_LIST
	../../common/include/common/net/config.h
	../../common/include/common/net/packetfactory.h
)

SET (COMMON_INCLUDE_LIST
	../../common/include/common/config.h
	../../common/include/common/setting.h
)

SET (COMMON_LIST
)



if (WIN32)
	source_group(SourceFiles\\character FILES ${SOURCEFILES_CHARACTER_LIST})
	source_group(SourceFiles\\engine\\thread\\net FILES ${SOURCEFILES_ENGINE_THREAD_NET_LIST})
	source_group(SourceFiles\\engine\\thread FILES ${SOURCEFILES_ENGINE_THREAD_LIST})
	source_group(SourceFiles\\engine FILES ${SOURCEFILES_ENGINE_LIST})
	source_group(SourceFiles\\connection\\counter FILES ${SOURCEFILES_CONNECTION_COUNTER_LIST})
	source_group(SourceFiles\\connection\\manager FILES ${SOURCEFILES_CONNECTION_MANAGER_LIST})
	source_group(SourceFiles\\connection\\queue FILES ${SOURCEFILES_CONNECTION_QUEUE_LIST})
	source_group(SourceFiles\\connection FILES ${SOURCEFILES_CONNECTION_LIST})
	source_group(SourceFiles\\db FILES ${SOURCEFILES_DB_LIST})
	source_group(SourceFiles\\message\\delegator FILES ${SOURCEFILES_MESSAGE_DELEGATOR_LIST})
	source_group(SourceFiles\\message\\handler\\client\\tologin FILES ${SOURCEFILES_MESSAGE_HANDLER_CLIENT_TOLOGIN_LIST})
	source_group(SourceFiles\\message\\handler\\gateway\\tologin FILES ${SOURCEFILES_MESSAGE_HANDLER_GATEWAY_TOLOGIN_LIST})
	source_group(SourceFiles\\message\\handler\\login\\tocenter FILES ${SOURCEFILES_MESSAGE_HANDLER_LOGIN_TOCENTER_LIST})
	source_group(SourceFiles\\message\\handler\\login\\toclient FILES ${SOURCEFILES_MESSAGE_HANDLER_LOGIN_TOCLIENT_LIST})
	source_group(SourceFiles\\message\\handler\\login\\togateway FILES ${SOURCEFILES_MESSAGE_HANDLER_LOGIN_TOGATEWAY_LIST})
	source_group(SourceFiles\\message\\handler\\serverserver FILES ${SOURCEFILES_MESSAGE_HANDLER_SERVERSERVER_LIST})
	source_group(SourceFiles\\message\\handler FILES ${SOURCEFILES_MESSAGE_HANDLER_LIST})
	source_group(SourceFiles\\message FILES ${SOURCEFILES_MESSAGE_LIST})
	source_group(SourceFiles FILES ${SOURCEFILES_LIST})
	source_group(HeaderFiles\\application FILES ${HEADERFILES_APPLICATION_LIST})
	source_group(HeaderFiles\\character FILES ${HEADERFILES_CHARACTER_LIST})
	source_group(HeaderFiles\\engine\\thread\\net FILES ${HEADERFILES_ENGINE_THREAD_NET_LIST})
	source_group(HeaderFiles\\engine\\thread FILES ${HEADERFILES_ENGINE_THREAD_LIST})
	source_group(HeaderFiles\\engine FILES ${HEADERFILES_ENGINE_LIST})
	source_group(HeaderFiles\\connection\\counter FILES ${HEADERFILES_CONNECTION_COUNTER_LIST})
	source_group(HeaderFiles\\connection\\manager FILES ${HEADERFILES_CONNECTION_MANAGER_LIST})
	source_group(HeaderFiles\\connection\\queue FILES ${HEADERFILES_CONNECTION_QUEUE_LIST})
	source_group(HeaderFiles\\connection FILES ${HEADERFILES_CONNECTION_LIST})
	source_group(HeaderFiles\\db FILES ${HEADERFILES_DB_LIST})
	source_group(HeaderFiles\\message\\delegator FILES ${HEADERFILES_MESSAGE_DELEGATOR_LIST})
	source_group(HeaderFiles\\message FILES ${HEADERFILES_MESSAGE_LIST})
	source_group(HeaderFiles FILES ${HEADERFILES_LIST})
	source_group(ResourceFiles FILES ${RESOURCEFILES_LIST})
	source_group(common\\src\\db FILES ${COMMON_SRC_DB_LIST})
	source_group(common\\src\\net\\connection FILES ${COMMON_SRC_NET_CONNECTION_LIST})
	source_group(common\\src\\net\\manager FILES ${COMMON_SRC_NET_MANAGER_LIST})
	source_group(common\\src\\net\\packet\\client\\tologin FILES ${COMMON_SRC_NET_PACKET_CLIENT_TOLOGIN_LIST})
	source_group(common\\src\\net\\packet\\login\\toclient FILES ${COMMON_SRC_NET_PACKET_LOGIN_TOCLIENT_LIST})
	source_group(common\\src\\net\\packet\\login\\tocenter FILES ${COMMON_SRC_NET_PACKET_LOGIN_TOCENTER_LIST})
	source_group(common\\src\\net\\packet\\center\\tologin FILES ${COMMON_SRC_NET_PACKET_CENTER_TOLOGIN_LIST})
	source_group(common\\src\\net\\packet\\login\\togateway FILES ${COMMON_SRC_NET_PACKET_LOGIN_TOGATEWAY_LIST})
	source_group(common\\src\\net\\packet\\gateway\\tologin FILES ${COMMON_SRC_NET_PACKET_GATEWAY_TOLOGIN_LIST})
	source_group(common\\src\\net\\packet\\serverserver FILES ${COMMON_SRC_NET_PACKET_SERVERSERVER_LIST})
	source_group(common\\src\\net\\packet FILES ${COMMON_SRC_NET_PACKET_LIST})
	source_group(common\\src\\net FILES ${COMMON_SRC_NET_LIST})
	source_group(common\\src FILES ${COMMON_SRC_LIST})
	source_group(common\\include\\define\\net\\packet\\id FILES ${COMMON_INCLUDE_DEFINE_NET_PACKET_ID_LIST})
	source_group(common\\include\\define\\net\\packet FILES ${COMMON_INCLUDE_DEFINE_NET_PACKET_LIST})
	source_group(common\\include\\define\\net FILES ${COMMON_INCLUDE_DEFINE_NET_LIST})
	source_group(common\\include\\define FILES ${COMMON_INCLUDE_DEFINE_LIST})
	source_group(common\\include\\db FILES ${COMMON_INCLUDE_DB_LIST})
	source_group(common\\include\\net\\connection FILES ${COMMON_INCLUDE_NET_CONNECTION_LIST})
	source_group(common\\include\\net\\manager FILES ${COMMON_INCLUDE_NET_MANAGER_LIST})
	source_group(common\\include\\net\\packet\\client\\tologin FILES ${COMMON_INCLUDE_NET_PACKET_CLIENT_TOLOGIN_LIST})
	source_group(common\\include\\net\\packet\\login\\toclient FILES ${COMMON_INCLUDE_NET_PACKET_LOGIN_TOCLIENT_LIST})
	source_group(common\\include\\net\\packet\\login\\tocenter FILES ${COMMON_INCLUDE_NET_PACKET_LOGIN_TOCENTER_LIST})
	source_group(common\\include\\net\\packet\\center\\tologin FILES ${COMMON_INCLUDE_NET_PACKET_CENTER_TOLOGIN_LIST})
	source_group(common\\include\\net\\packet\\login\\togateway FILES ${COMMON_INCLUDE_NET_PACKET_LOGIN_TOGATEWAY_LIST})
	source_group(common\\include\\net\\packet\\gateway\\tologin FILES ${COMMON_INCLUDE_NET_PACKET_GATEWAY_TOLOGIN_LIST})
	source_group(common\\include\\net\\packet\\serverserver FILES ${COMMON_INCLUDE_NET_PACKET_SERVERSERVER_LIST})
	source_group(common\\include\\net\\packet FILES ${COMMON_INCLUDE_NET_PACKET_LIST})
	source_group(common\\include\\net FILES ${COMMON_INCLUDE_NET_LIST})
	source_group(common\\include FILES ${COMMON_INCLUDE_LIST})
	source_group(common FILES ${COMMON_LIST})
endif()


ADD_EXECUTABLE(login
	${SOURCEFILES_CHARACTER_LIST}
	${SOURCEFILES_ENGINE_THREAD_NET_LIST}
	${SOURCEFILES_ENGINE_THREAD_LIST}
	${SOURCEFILES_ENGINE_LIST}
	${SOURCEFILES_CONNECTION_COUNTER_LIST}
	${SOURCEFILES_CONNECTION_MANAGER_LIST}
	${SOURCEFILES_CONNECTION_QUEUE_LIST}
	${SOURCEFILES_CONNECTION_LIST}
	${SOURCEFILES_DB_LIST}
	${SOURCEFILES_MESSAGE_DELEGATOR_LIST}
	${SOURCEFILES_MESSAGE_HANDLER_CLIENT_TOLOGIN_LIST}
	${SOURCEFILES_MESSAGE_HANDLER_GATEWAY_TOLOGIN_LIST}
	${SOURCEFILES_MESSAGE_HANDLER_LOGIN_TOCENTER_LIST}
	${SOURCEFILES_MESSAGE_HANDLER_LOGIN_TOCLIENT_LIST}
	${SOURCEFILES_MESSAGE_HANDLER_LOGIN_TOGATEWAY_LIST}
	${SOURCEFILES_MESSAGE_HANDLER_SERVERSERVER_LIST}
	${SOURCEFILES_MESSAGE_HANDLER_LIST}
	${SOURCEFILES_MESSAGE_LIST}
	${SOURCEFILES_LIST}
	${HEADERFILES_APPLICATION_LIST}
	${HEADERFILES_CHARACTER_LIST}
	${HEADERFILES_ENGINE_THREAD_NET_LIST}
	${HEADERFILES_ENGINE_THREAD_LIST}
	${HEADERFILES_ENGINE_LIST}
	${HEADERFILES_CONNECTION_COUNTER_LIST}
	${HEADERFILES_CONNECTION_MANAGER_LIST}
	${HEADERFILES_CONNECTION_QUEUE_LIST}
	${HEADERFILES_CONNECTION_LIST}
	${HEADERFILES_DB_LIST}
	${HEADERFILES_MESSAGE_DELEGATOR_LIST}
	${HEADERFILES_MESSAGE_LIST}
	${HEADERFILES_LIST}
	${RESOURCEFILES_LIST}
	${COMMON_SRC_DB_LIST}
	${COMMON_SRC_NET_CONNECTION_LIST}
	${COMMON_SRC_NET_MANAGER_LIST}
	${COMMON_SRC_NET_PACKET_CLIENT_TOLOGIN_LIST}
	${COMMON_SRC_NET_PACKET_LOGIN_TOCLIENT_LIST}
	${COMMON_SRC_NET_PACKET_LOGIN_TOCENTER_LIST}
	${COMMON_SRC_NET_PACKET_CENTER_TOLOGIN_LIST}
	${COMMON_SRC_NET_PACKET_LOGIN_TOGATEWAY_LIST}
	${COMMON_SRC_NET_PACKET_GATEWAY_TOLOGIN_LIST}
	${COMMON_SRC_NET_PACKET_SERVERSERVER_LIST}
	${COMMON_SRC_NET_PACKET_LIST}
	${COMMON_SRC_NET_LIST}
	${COMMON_SRC_LIST}
	${COMMON_INCLUDE_DEFINE_NET_PACKET_ID_LIST}
	${COMMON_INCLUDE_DEFINE_NET_PACKET_LIST}
	${COMMON_INCLUDE_DEFINE_NET_LIST}
	${COMMON_INCLUDE_DEFINE_LIST}
	${COMMON_INCLUDE_DB_LIST}
	${COMMON_INCLUDE_NET_CONNECTION_LIST}
	${COMMON_INCLUDE_NET_MANAGER_LIST}
	${COMMON_INCLUDE_NET_PACKET_CLIENT_TOLOGIN_LIST}
	${COMMON_INCLUDE_NET_PACKET_LOGIN_TOCLIENT_LIST}
	${COMMON_INCLUDE_NET_PACKET_LOGIN_TOCENTER_LIST}
	${COMMON_INCLUDE_NET_PACKET_CENTER_TOLOGIN_LIST}
	${COMMON_INCLUDE_NET_PACKET_LOGIN_TOGATEWAY_LIST}
	${COMMON_INCLUDE_NET_PACKET_GATEWAY_TOLOGIN_LIST}
	${COMMON_INCLUDE_NET_PACKET_SERVERSERVER_LIST}
	${COMMON_INCLUDE_NET_PACKET_LIST}
	${COMMON_INCLUDE_NET_LIST}
	${COMMON_INCLUDE_LIST}
	${COMMON_LIST}
)


##update_end

if(USE_32BITS)
  SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=i386 -mtune=i686")
  SET(CMAKE_CXX_FLAGS "-Wall -ggdb -pipe -march=i386 -mtune=i686")
else()
  SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=x86-64 -mtune=i686")
  SET(CMAKE_CXX_FLAGS "-Wall -ggdb -pipe -march=x86-64 -mtune=i686")
endif(USE_32BITS)

if (WIN32)
TARGET_LINK_LIBRARIES(login ws2_32.lib odbc32.lib odbccp32.lib pf_core.lib)
else()
TARGET_LINK_LIBRARIES(login pthread odbc pf_core)
  if(USE_32BITS)
    SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=i386 -mtune=i686")
    SET(CMAKE_CXX_FLAGS "-Wall -ggdb -pipe -march=i386 -mtune=i686")
  else()
    SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=x86-64 -mtune=i686")
    SET(CMAKE_CXX_FLAGS "-Wall -ggdb -pipe -march=x86-64 -mtune=i686")
  endif(USE_32BITS)
endif(WIN32)
