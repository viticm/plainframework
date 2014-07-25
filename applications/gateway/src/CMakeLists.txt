SET(TF_CURRENT_BINARY_PATH_BIN "../../bin")
ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE)
ADD_DEFINITIONS(-D_PS_GATEWAY)
ADD_DEFINITIONS(-D_PS_NET_GATEWAY)

IF(CMAKE_SYSTEM MATCHES Linux)
  ADD_DEFINITIONS(-D_REENTRANT)
  ADD_DEFINITIONS(-DDONT_TD_VOID)
  ADD_DEFINITIONS(-D_PS_NET_EPOLL)
ELSE(CMAKE_SYSTEM MATCHES Linux)
ENDIF(CMAKE_SYSTEM MATCHES Linux)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})
LINK_DIRECTORIES(
  "./" #run dir
  "../../lib/lua/lib" #linux
  "../../lib/libiconv/lib" #win32
)

SET(EXECUTABLE_OUTPUT_PATH ${TF_CURRENT_BINARY_PATH_BIN})


INCLUDE_DIRECTORIES("../../common/include",
                    "../include",
                    "../../lib/lua/src/",
                    "../../lib/libiconv/include",
                    "/usr/local/unixODBC/include")

##update_begin




SET (SOURCEFILES_COMMON_BASE_HASHMAP_LIST
	../../common/src/common/base/hashmap/integer.cc
)

SET (SOURCEFILES_COMMON_BASE_LIST
	../../common/src/common/base/io.cc
	../../common/src/common/base/log.cc
	../../common/src/common/base/md5.cc
	../../common/src/common/base/string.cc
	../../common/src/common/base/time_manager.cc
	../../common/src/common/base/util.cc
)

SET (SOURCEFILES_COMMON_SYS_MEMORY_LIST
	../../common/src/common/sys/memory/dynamic_allocator.cc
	../../common/src/common/sys/memory/share.cc
	../../common/src/common/sys/memory/static_allocator.cc
)

SET (SOURCEFILES_COMMON_SYS_LIST
	../../common/src/common/sys/assert.cc
	../../common/src/common/sys/info.cc
	../../common/src/common/sys/minidump.cc
	../../common/src/common/sys/process.cc
	../../common/src/common/sys/thread.cc
	../../common/src/common/sys/util.cc
)

SET (SOURCEFILES_COMMON_FILE_LIST
	../../common/src/common/file/api.cc
	../../common/src/common/file/database.cc
	../../common/src/common/file/ini.cc
)

SET (SOURCEFILES_COMMON_NET_PACKET_LIST
	../../common/src/common/net/packet/base.cc
	../../common/src/common/net/packet/factorymanager.cc
)

SET (SOURCEFILES_COMMON_NET_SOCKET_LIST
	../../common/src/common/net/socket/api.cc
	../../common/src/common/net/socket/base.cc
	../../common/src/common/net/socket/encode.cc
	../../common/src/common/net/socket/inputstream.cc
	../../common/src/common/net/socket/outputstream.cc
	../../common/src/common/net/socket/server.cc
	../../common/src/common/net/socket/stream.cc
)

SET (SOURCEFILES_COMMON_NET_CONNECTION_MANAGER_LIST
	../../common/src/common/net/connection/manager/base.cc
	../../common/src/common/net/connection/manager/epoll.cc
	../../common/src/common/net/connection/manager/iocp.cc
	../../common/src/common/net/connection/manager/select.cc
)

SET (SOURCEFILES_COMMON_NET_CONNECTION_LIST
	../../common/src/common/net/connection/base.cc
	../../common/src/common/net/connection/pool.cc
	../../common/src/common/net/connection/server.cc
)

SET (SOURCEFILES_COMMON_NET_LIST
	../../common/src/common/net/manager.cc
)

SET (SOURCEFILES_COMMON_ENGINE_THREAD_LIST
	../../common/src/common/engine/thread/db.cc
	../../common/src/common/engine/thread/net.cc
	../../common/src/common/engine/thread/performance.cc
	../../common/src/common/engine/thread/script.cc
)

SET (SOURCEFILES_COMMON_ENGINE_LIST
	../../common/src/common/engine/kernel.cc
)

SET (SOURCEFILES_COMMON_DB_ODBC_LIST
	../../common/src/common/db/odbc/interface.cc
	../../common/src/common/db/odbc/system.cc
)

SET (SOURCEFILES_COMMON_DB_LIST
	../../common/src/common/db/manager.cc
)

SET (SOURCEFILES_COMMON_SCRIPT_CACHE_LIST
	../../common/src/common/script/cache/base.cc
	../../common/src/common/script/cache/manager.cc
)

SET (SOURCEFILES_COMMON_SCRIPT_LUA_LIST
	../../common/src/common/script/lua/filebridge.cc
	../../common/src/common/script/lua/interface.cc
	../../common/src/common/script/lua/lunar.cc
	../../common/src/common/script/lua/stack.cc
	../../common/src/common/script/lua/system.cc
	../../common/src/common/script/lua/vm.cc
)

SET (SOURCEFILES_COMMON_SCRIPT_LIST
)

SET (SOURCEFILES_COMMON_PERFORMANCE_LIST
	../../common/src/common/performance/eyes.cc
)

SET (SOURCEFILES_COMMON_UTIL_COMPRESSOR_LIST
	../../common/src/common/util/compressor/mini.cc
	../../common/src/common/util/compressor/minilzo.c
)

SET (SOURCEFILES_COMMON_UTIL_LIST
)

SET (SOURCEFILES_COMMON_APPLICATION_NET_PACKET_GATEWAY_TOLOGIN_LIST
	../../common/src/common/application/net/packet/gateway_tologin/resultauth.cc
)

SET (SOURCEFILES_COMMON_APPLICATION_NET_PACKET_LOGIN_TOGATEWAY_LIST
	../../common/src/common/application/net/packet/login_togateway/askauth.cc
)

SET (SOURCEFILES_COMMON_APPLICATION_NET_PACKET_SERVERSERVER_LIST
	../../common/src/common/application/net/packet/serverserver/connect.cc
)

SET (SOURCEFILES_COMMON_APPLICATION_NET_PACKET_LIST
)

SET (SOURCEFILES_COMMON_APPLICATION_NET_LIST
)

SET (SOURCEFILES_COMMON_APPLICATION_EXTEND_LIST
	../../common/src/common/application/extend/log.cc
	../../common/src/common/application/extend/net_packet_factorymanager.cc
)

SET (SOURCEFILES_COMMON_APPLICATION_LIST
	../../common/src/common/application/setting.cc
)

SET (SOURCEFILES_COMMON_LIST
)

SET (SOURCEFILES_MESSAGE_HANDLER_SERVERSERVER_LIST
	../src/message/handler/serverserver/connect.cc
)

SET (SOURCEFILES_MESSAGE_HANDLER_LOGIN_TOGATEWAY_LIST
	../src/message/handler/login_togateway/askauth.cc
)

SET (SOURCEFILES_MESSAGE_HANDLER_GATEWAY_TOLOGIN_LIST
	../src/message/handler/gateway_tologin/resultauth.cc
)

SET (SOURCEFILES_MESSAGE_HANDLER_LIST
)

SET (SOURCEFILES_MESSAGE_DELEGATOR_LIST
	../src/message/delegator/account.cc
)

SET (SOURCEFILES_MESSAGE_LIST
)

SET (SOURCEFILES_DB_LIST
	../src/db/user.cc
)

SET (SOURCEFILES_ENGINE_LIST
	../src/engine/system.cc
)

SET (SOURCEFILES_LIST
	../src/gateway.cc
)

SET (HEADERFILES_COMMON_BASE_HASHMAP_LIST
	../../common/include/common/base/hashmap/config.h
	../../common/include/common/base/hashmap/integer.h
	../../common/include/common/base/hashmap/template.h
)

SET (HEADERFILES_COMMON_BASE_LIST
	../../common/include/common/base/base64.hpp
	../../common/include/common/base/config.h
	../../common/include/common/base/io.h
	../../common/include/common/base/log.h
	../../common/include/common/base/md5.h
	../../common/include/common/base/singleton.h
	../../common/include/common/base/string.h
	../../common/include/common/base/time_manager.h
	../../common/include/common/base/type.h
	../../common/include/common/base/util.h
)

SET (HEADERFILES_COMMON_DB_ODBC_LIST
	../../common/include/common/db/odbc/config.h
	../../common/include/common/db/odbc/interface.h
	../../common/include/common/db/odbc/system.h
)

SET (HEADERFILES_COMMON_DB_LIST
	../../common/include/common/db/config.h
	../../common/include/common/db/manager.h
)

SET (HEADERFILES_COMMON_FILE_LIST
	../../common/include/common/file/api.h
	../../common/include/common/file/config.h
	../../common/include/common/file/database.h
	../../common/include/common/file/ini.h
)

SET (HEADERFILES_COMMON_NET_SOCKET_LIST
	../../common/include/common/net/socket/api.h
	../../common/include/common/net/socket/base.h
	../../common/include/common/net/socket/config.h
	../../common/include/common/net/socket/encode.h
	../../common/include/common/net/socket/extend.inl
	../../common/include/common/net/socket/inputstream.h
	../../common/include/common/net/socket/outputstream.h
	../../common/include/common/net/socket/server.h
	../../common/include/common/net/socket/stream.h
)

SET (HEADERFILES_COMMON_NET_PACKET_LIST
	../../common/include/common/net/packet/base.h
	../../common/include/common/net/packet/config.h
	../../common/include/common/net/packet/factory.h
	../../common/include/common/net/packet/factorymanager.h
)

SET (HEADERFILES_COMMON_NET_CONNECTION_MANAGER_LIST
	../../common/include/common/net/connection/manager/base.h
	../../common/include/common/net/connection/manager/config.h
	../../common/include/common/net/connection/manager/epoll.h
	../../common/include/common/net/connection/manager/iocp.h
	../../common/include/common/net/connection/manager/select.h
)

SET (HEADERFILES_COMMON_NET_CONNECTION_LIST
	../../common/include/common/net/connection/base.h
	../../common/include/common/net/connection/config.h
	../../common/include/common/net/connection/pool.h
	../../common/include/common/net/connection/server.h
)

SET (HEADERFILES_COMMON_NET_LIST
	../../common/include/common/net/config.h
	../../common/include/common/net/manager.h
)

SET (HEADERFILES_COMMON_SCRIPT_CACHE_LIST
	../../common/include/common/script/cache/base.h
	../../common/include/common/script/cache/config.h
	../../common/include/common/script/cache/manager.h
)

SET (HEADERFILES_COMMON_SCRIPT_LUA_LIST
	../../common/include/common/script/lua/config.h
	../../common/include/common/script/lua/filebridge.h
	../../common/include/common/script/lua/interface.h
	../../common/include/common/script/lua/lunar.h
	../../common/include/common/script/lua/stack.h
	../../common/include/common/script/lua/system.h
	../../common/include/common/script/lua/vm.h
)

SET (HEADERFILES_COMMON_SCRIPT_LIST
	../../common/include/common/script/config.h
)

SET (HEADERFILES_COMMON_ENGINE_THREAD_LIST
	../../common/include/common/engine/thread/config.h
	../../common/include/common/engine/thread/db.h
	../../common/include/common/engine/thread/net.h
	../../common/include/common/engine/thread/performance.h
	../../common/include/common/engine/thread/script.h
)

SET (HEADERFILES_COMMON_ENGINE_LIST
	../../common/include/common/engine/config.h
	../../common/include/common/engine/kernel.h
)

SET (HEADERFILES_COMMON_SYS_MEMORY_LIST
	../../common/include/common/sys/memory/config.h
	../../common/include/common/sys/memory/dynamic_allocator.h
	../../common/include/common/sys/memory/share.h
	../../common/include/common/sys/memory/static_allocator.h
)

SET (HEADERFILES_COMMON_SYS_LIST
	../../common/include/common/sys/assert.h
	../../common/include/common/sys/config.h
	../../common/include/common/sys/info.h
	../../common/include/common/sys/minidump.h
	../../common/include/common/sys/process.h
	../../common/include/common/sys/thread.h
	../../common/include/common/sys/util.h
)

SET (HEADERFILES_COMMON_PERFORMANCE_LIST
	../../common/include/common/performance/config.h
	../../common/include/common/performance/eyes.h
)

SET (HEADERFILES_COMMON_UTIL_COMPRESSOR_LIST
	../../common/include/common/util/compressor/config.h
	../../common/include/common/util/compressor/lzoconf.h
	../../common/include/common/util/compressor/lzodefs.h
	../../common/include/common/util/compressor/mini.h
	../../common/include/common/util/compressor/minilzo.h
)

SET (HEADERFILES_COMMON_UTIL_LIST
	../../common/include/common/util/config.h
)

SET (HEADERFILES_COMMON_APPLICATION_DEFINE_NET_PACKET_ID_LIST
	../../common/include/common/application/define/net/packet/id/all.h
	../../common/include/common/application/define/net/packet/id/clientlogin.h
	../../common/include/common/application/define/net/packet/id/clientserver.h
	../../common/include/common/application/define/net/packet/id/config.h
	../../common/include/common/application/define/net/packet/id/gatewaylogin.h
	../../common/include/common/application/define/net/packet/id/logincenter.h
	../../common/include/common/application/define/net/packet/id/servercenter.h
	../../common/include/common/application/define/net/packet/id/serverserver.h
)

SET (HEADERFILES_COMMON_APPLICATION_DEFINE_NET_PACKET_LIST
	../../common/include/common/application/define/net/packet/config.h
)

SET (HEADERFILES_COMMON_APPLICATION_DEFINE_NET_LIST
	../../common/include/common/application/define/net/config.h
)

SET (HEADERFILES_COMMON_APPLICATION_DEFINE_LIST
	../../common/include/common/application/define/all.h
	../../common/include/common/application/define/config.h
	../../common/include/common/application/define/file.h
	../../common/include/common/application/define/log.h
	../../common/include/common/application/define/macros.h
)

SET (HEADERFILES_COMMON_APPLICATION_NET_PACKET_LOGIN_TOGATEWAY_LIST
	../../common/include/common/application/net/packet/login_togateway/askauth.h
	../../common/include/common/application/net/packet/login_togateway/config.h
)

SET (HEADERFILES_COMMON_APPLICATION_NET_PACKET_SERVERSERVER_LIST
	../../common/include/common/application/net/packet/serverserver/config.h
	../../common/include/common/application/net/packet/serverserver/connect.h
)

SET (HEADERFILES_COMMON_APPLICATION_NET_PACKET_GATEWAY_TOLOGIN_LIST
	../../common/include/common/application/net/packet/gateway_tologin/config.h
	../../common/include/common/application/net/packet/gateway_tologin/resultauth.h
)

SET (HEADERFILES_COMMON_APPLICATION_NET_PACKET_LIST
	../../common/include/common/application/net/packet/config.h
)

SET (HEADERFILES_COMMON_APPLICATION_NET_LIST
	../../common/include/common/application/net/config.h
)

SET (HEADERFILES_COMMON_APPLICATION_EXTEND_LIST
	../../common/include/common/application/extend/config.h
	../../common/include/common/application/extend/define.h
	../../common/include/common/application/extend/log.h
	../../common/include/common/application/extend/net_packet_factorymanager.h
)

SET (HEADERFILES_COMMON_APPLICATION_LIST
	../../common/include/common/application/config.h
	../../common/include/common/application/setting.h
)

SET (HEADERFILES_COMMON_LIST
)

SET (HEADERFILES_CONFIG_LIST
	../include/config/application.h
)

SET (HEADERFILES_DB_LIST
	../include/db/config.h
	../include/db/user.h
)

SET (HEADERFILES_ENGINE_LIST
	../include/engine/config.h
	../include/engine/system.h
)

SET (HEADERFILES_MESSAGE_DELEGATOR_LIST
	../include/message/delegator/account.h
	../include/message/delegator/config.h
)

SET (HEADERFILES_MESSAGE_LIST
	../include/message/config.h
)

SET (HEADERFILES_LIST
	../include/gateway.h
)

SET (RESOURCEFILES_LIST
)



if (WIN32)
	source_group(SourceFiles\\common\\base\\hashmap FILES ${SOURCEFILES_COMMON_BASE_HASHMAP_LIST})
	source_group(SourceFiles\\common\\base FILES ${SOURCEFILES_COMMON_BASE_LIST})
	source_group(SourceFiles\\common\\sys\\memory FILES ${SOURCEFILES_COMMON_SYS_MEMORY_LIST})
	source_group(SourceFiles\\common\\sys FILES ${SOURCEFILES_COMMON_SYS_LIST})
	source_group(SourceFiles\\common\\file FILES ${SOURCEFILES_COMMON_FILE_LIST})
	source_group(SourceFiles\\common\\net\\packet FILES ${SOURCEFILES_COMMON_NET_PACKET_LIST})
	source_group(SourceFiles\\common\\net\\socket FILES ${SOURCEFILES_COMMON_NET_SOCKET_LIST})
	source_group(SourceFiles\\common\\net\\connection\\manager FILES ${SOURCEFILES_COMMON_NET_CONNECTION_MANAGER_LIST})
	source_group(SourceFiles\\common\\net\\connection FILES ${SOURCEFILES_COMMON_NET_CONNECTION_LIST})
	source_group(SourceFiles\\common\\net FILES ${SOURCEFILES_COMMON_NET_LIST})
	source_group(SourceFiles\\common\\engine\\thread FILES ${SOURCEFILES_COMMON_ENGINE_THREAD_LIST})
	source_group(SourceFiles\\common\\engine FILES ${SOURCEFILES_COMMON_ENGINE_LIST})
	source_group(SourceFiles\\common\\db\\odbc FILES ${SOURCEFILES_COMMON_DB_ODBC_LIST})
	source_group(SourceFiles\\common\\db FILES ${SOURCEFILES_COMMON_DB_LIST})
	source_group(SourceFiles\\common\\script\\cache FILES ${SOURCEFILES_COMMON_SCRIPT_CACHE_LIST})
	source_group(SourceFiles\\common\\script\\lua FILES ${SOURCEFILES_COMMON_SCRIPT_LUA_LIST})
	source_group(SourceFiles\\common\\script FILES ${SOURCEFILES_COMMON_SCRIPT_LIST})
	source_group(SourceFiles\\common\\performance FILES ${SOURCEFILES_COMMON_PERFORMANCE_LIST})
	source_group(SourceFiles\\common\\util\\compressor FILES ${SOURCEFILES_COMMON_UTIL_COMPRESSOR_LIST})
	source_group(SourceFiles\\common\\util FILES ${SOURCEFILES_COMMON_UTIL_LIST})
	source_group(SourceFiles\\common\\application\\net\\packet\\gateway\\tologin FILES ${SOURCEFILES_COMMON_APPLICATION_NET_PACKET_GATEWAY_TOLOGIN_LIST})
	source_group(SourceFiles\\common\\application\\net\\packet\\login\\togateway FILES ${SOURCEFILES_COMMON_APPLICATION_NET_PACKET_LOGIN_TOGATEWAY_LIST})
	source_group(SourceFiles\\common\\application\\net\\packet\\serverserver FILES ${SOURCEFILES_COMMON_APPLICATION_NET_PACKET_SERVERSERVER_LIST})
	source_group(SourceFiles\\common\\application\\net\\packet FILES ${SOURCEFILES_COMMON_APPLICATION_NET_PACKET_LIST})
	source_group(SourceFiles\\common\\application\\net FILES ${SOURCEFILES_COMMON_APPLICATION_NET_LIST})
	source_group(SourceFiles\\common\\application\\extend FILES ${SOURCEFILES_COMMON_APPLICATION_EXTEND_LIST})
	source_group(SourceFiles\\common\\application FILES ${SOURCEFILES_COMMON_APPLICATION_LIST})
	source_group(SourceFiles\\common FILES ${SOURCEFILES_COMMON_LIST})
	source_group(SourceFiles\\message\\handler\\serverserver FILES ${SOURCEFILES_MESSAGE_HANDLER_SERVERSERVER_LIST})
	source_group(SourceFiles\\message\\handler\\login\\togateway FILES ${SOURCEFILES_MESSAGE_HANDLER_LOGIN_TOGATEWAY_LIST})
	source_group(SourceFiles\\message\\handler\\gateway\\tologin FILES ${SOURCEFILES_MESSAGE_HANDLER_GATEWAY_TOLOGIN_LIST})
	source_group(SourceFiles\\message\\handler FILES ${SOURCEFILES_MESSAGE_HANDLER_LIST})
	source_group(SourceFiles\\message\\delegator FILES ${SOURCEFILES_MESSAGE_DELEGATOR_LIST})
	source_group(SourceFiles\\message FILES ${SOURCEFILES_MESSAGE_LIST})
	source_group(SourceFiles\\db FILES ${SOURCEFILES_DB_LIST})
	source_group(SourceFiles\\engine FILES ${SOURCEFILES_ENGINE_LIST})
	source_group(SourceFiles FILES ${SOURCEFILES_LIST})
	source_group(HeaderFiles\\common\\base\\hashmap FILES ${HEADERFILES_COMMON_BASE_HASHMAP_LIST})
	source_group(HeaderFiles\\common\\base FILES ${HEADERFILES_COMMON_BASE_LIST})
	source_group(HeaderFiles\\common\\db\\odbc FILES ${HEADERFILES_COMMON_DB_ODBC_LIST})
	source_group(HeaderFiles\\common\\db FILES ${HEADERFILES_COMMON_DB_LIST})
	source_group(HeaderFiles\\common\\file FILES ${HEADERFILES_COMMON_FILE_LIST})
	source_group(HeaderFiles\\common\\net\\socket FILES ${HEADERFILES_COMMON_NET_SOCKET_LIST})
	source_group(HeaderFiles\\common\\net\\packet FILES ${HEADERFILES_COMMON_NET_PACKET_LIST})
	source_group(HeaderFiles\\common\\net\\connection\\manager FILES ${HEADERFILES_COMMON_NET_CONNECTION_MANAGER_LIST})
	source_group(HeaderFiles\\common\\net\\connection FILES ${HEADERFILES_COMMON_NET_CONNECTION_LIST})
	source_group(HeaderFiles\\common\\net FILES ${HEADERFILES_COMMON_NET_LIST})
	source_group(HeaderFiles\\common\\script\\cache FILES ${HEADERFILES_COMMON_SCRIPT_CACHE_LIST})
	source_group(HeaderFiles\\common\\script\\lua FILES ${HEADERFILES_COMMON_SCRIPT_LUA_LIST})
	source_group(HeaderFiles\\common\\script FILES ${HEADERFILES_COMMON_SCRIPT_LIST})
	source_group(HeaderFiles\\common\\engine\\thread FILES ${HEADERFILES_COMMON_ENGINE_THREAD_LIST})
	source_group(HeaderFiles\\common\\engine FILES ${HEADERFILES_COMMON_ENGINE_LIST})
	source_group(HeaderFiles\\common\\sys\\memory FILES ${HEADERFILES_COMMON_SYS_MEMORY_LIST})
	source_group(HeaderFiles\\common\\sys FILES ${HEADERFILES_COMMON_SYS_LIST})
	source_group(HeaderFiles\\common\\performance FILES ${HEADERFILES_COMMON_PERFORMANCE_LIST})
	source_group(HeaderFiles\\common\\util\\compressor FILES ${HEADERFILES_COMMON_UTIL_COMPRESSOR_LIST})
	source_group(HeaderFiles\\common\\util FILES ${HEADERFILES_COMMON_UTIL_LIST})
	source_group(HeaderFiles\\common\\application\\define\\net\\packet\\id FILES ${HEADERFILES_COMMON_APPLICATION_DEFINE_NET_PACKET_ID_LIST})
	source_group(HeaderFiles\\common\\application\\define\\net\\packet FILES ${HEADERFILES_COMMON_APPLICATION_DEFINE_NET_PACKET_LIST})
	source_group(HeaderFiles\\common\\application\\define\\net FILES ${HEADERFILES_COMMON_APPLICATION_DEFINE_NET_LIST})
	source_group(HeaderFiles\\common\\application\\define FILES ${HEADERFILES_COMMON_APPLICATION_DEFINE_LIST})
	source_group(HeaderFiles\\common\\application\\net\\packet\\login\\togateway FILES ${HEADERFILES_COMMON_APPLICATION_NET_PACKET_LOGIN_TOGATEWAY_LIST})
	source_group(HeaderFiles\\common\\application\\net\\packet\\serverserver FILES ${HEADERFILES_COMMON_APPLICATION_NET_PACKET_SERVERSERVER_LIST})
	source_group(HeaderFiles\\common\\application\\net\\packet\\gateway\\tologin FILES ${HEADERFILES_COMMON_APPLICATION_NET_PACKET_GATEWAY_TOLOGIN_LIST})
	source_group(HeaderFiles\\common\\application\\net\\packet FILES ${HEADERFILES_COMMON_APPLICATION_NET_PACKET_LIST})
	source_group(HeaderFiles\\common\\application\\net FILES ${HEADERFILES_COMMON_APPLICATION_NET_LIST})
	source_group(HeaderFiles\\common\\application\\extend FILES ${HEADERFILES_COMMON_APPLICATION_EXTEND_LIST})
	source_group(HeaderFiles\\common\\application FILES ${HEADERFILES_COMMON_APPLICATION_LIST})
	source_group(HeaderFiles\\common FILES ${HEADERFILES_COMMON_LIST})
	source_group(HeaderFiles\\config FILES ${HEADERFILES_CONFIG_LIST})
	source_group(HeaderFiles\\db FILES ${HEADERFILES_DB_LIST})
	source_group(HeaderFiles\\engine FILES ${HEADERFILES_ENGINE_LIST})
	source_group(HeaderFiles\\message\\delegator FILES ${HEADERFILES_MESSAGE_DELEGATOR_LIST})
	source_group(HeaderFiles\\message FILES ${HEADERFILES_MESSAGE_LIST})
	source_group(HeaderFiles FILES ${HEADERFILES_LIST})
	source_group(ResourceFiles FILES ${RESOURCEFILES_LIST})
endif()


ADD_EXECUTABLE(gateway
	${SOURCEFILES_COMMON_BASE_HASHMAP_LIST}
	${SOURCEFILES_COMMON_BASE_LIST}
	${SOURCEFILES_COMMON_SYS_MEMORY_LIST}
	${SOURCEFILES_COMMON_SYS_LIST}
	${SOURCEFILES_COMMON_FILE_LIST}
	${SOURCEFILES_COMMON_NET_PACKET_LIST}
	${SOURCEFILES_COMMON_NET_SOCKET_LIST}
	${SOURCEFILES_COMMON_NET_CONNECTION_MANAGER_LIST}
	${SOURCEFILES_COMMON_NET_CONNECTION_LIST}
	${SOURCEFILES_COMMON_NET_LIST}
	${SOURCEFILES_COMMON_ENGINE_THREAD_LIST}
	${SOURCEFILES_COMMON_ENGINE_LIST}
	${SOURCEFILES_COMMON_DB_ODBC_LIST}
	${SOURCEFILES_COMMON_DB_LIST}
	${SOURCEFILES_COMMON_SCRIPT_CACHE_LIST}
	${SOURCEFILES_COMMON_SCRIPT_LUA_LIST}
	${SOURCEFILES_COMMON_SCRIPT_LIST}
	${SOURCEFILES_COMMON_PERFORMANCE_LIST}
	${SOURCEFILES_COMMON_UTIL_COMPRESSOR_LIST}
	${SOURCEFILES_COMMON_UTIL_LIST}
	${SOURCEFILES_COMMON_APPLICATION_NET_PACKET_GATEWAY_TOLOGIN_LIST}
	${SOURCEFILES_COMMON_APPLICATION_NET_PACKET_LOGIN_TOGATEWAY_LIST}
	${SOURCEFILES_COMMON_APPLICATION_NET_PACKET_SERVERSERVER_LIST}
	${SOURCEFILES_COMMON_APPLICATION_NET_PACKET_LIST}
	${SOURCEFILES_COMMON_APPLICATION_NET_LIST}
	${SOURCEFILES_COMMON_APPLICATION_EXTEND_LIST}
	${SOURCEFILES_COMMON_APPLICATION_LIST}
	${SOURCEFILES_COMMON_LIST}
	${SOURCEFILES_MESSAGE_HANDLER_SERVERSERVER_LIST}
	${SOURCEFILES_MESSAGE_HANDLER_LOGIN_TOGATEWAY_LIST}
	${SOURCEFILES_MESSAGE_HANDLER_GATEWAY_TOLOGIN_LIST}
	${SOURCEFILES_MESSAGE_HANDLER_LIST}
	${SOURCEFILES_MESSAGE_DELEGATOR_LIST}
	${SOURCEFILES_MESSAGE_LIST}
	${SOURCEFILES_DB_LIST}
	${SOURCEFILES_ENGINE_LIST}
	${SOURCEFILES_LIST}
	${HEADERFILES_COMMON_BASE_HASHMAP_LIST}
	${HEADERFILES_COMMON_BASE_LIST}
	${HEADERFILES_COMMON_DB_ODBC_LIST}
	${HEADERFILES_COMMON_DB_LIST}
	${HEADERFILES_COMMON_FILE_LIST}
	${HEADERFILES_COMMON_NET_SOCKET_LIST}
	${HEADERFILES_COMMON_NET_PACKET_LIST}
	${HEADERFILES_COMMON_NET_CONNECTION_MANAGER_LIST}
	${HEADERFILES_COMMON_NET_CONNECTION_LIST}
	${HEADERFILES_COMMON_NET_LIST}
	${HEADERFILES_COMMON_SCRIPT_CACHE_LIST}
	${HEADERFILES_COMMON_SCRIPT_LUA_LIST}
	${HEADERFILES_COMMON_SCRIPT_LIST}
	${HEADERFILES_COMMON_ENGINE_THREAD_LIST}
	${HEADERFILES_COMMON_ENGINE_LIST}
	${HEADERFILES_COMMON_SYS_MEMORY_LIST}
	${HEADERFILES_COMMON_SYS_LIST}
	${HEADERFILES_COMMON_PERFORMANCE_LIST}
	${HEADERFILES_COMMON_UTIL_COMPRESSOR_LIST}
	${HEADERFILES_COMMON_UTIL_LIST}
	${HEADERFILES_COMMON_APPLICATION_DEFINE_NET_PACKET_ID_LIST}
	${HEADERFILES_COMMON_APPLICATION_DEFINE_NET_PACKET_LIST}
	${HEADERFILES_COMMON_APPLICATION_DEFINE_NET_LIST}
	${HEADERFILES_COMMON_APPLICATION_DEFINE_LIST}
	${HEADERFILES_COMMON_APPLICATION_NET_PACKET_LOGIN_TOGATEWAY_LIST}
	${HEADERFILES_COMMON_APPLICATION_NET_PACKET_SERVERSERVER_LIST}
	${HEADERFILES_COMMON_APPLICATION_NET_PACKET_GATEWAY_TOLOGIN_LIST}
	${HEADERFILES_COMMON_APPLICATION_NET_PACKET_LIST}
	${HEADERFILES_COMMON_APPLICATION_NET_LIST}
	${HEADERFILES_COMMON_APPLICATION_EXTEND_LIST}
	${HEADERFILES_COMMON_APPLICATION_LIST}
	${HEADERFILES_COMMON_LIST}
	${HEADERFILES_CONFIG_LIST}
	${HEADERFILES_DB_LIST}
	${HEADERFILES_ENGINE_LIST}
	${HEADERFILES_MESSAGE_DELEGATOR_LIST}
	${HEADERFILES_MESSAGE_LIST}
	${HEADERFILES_LIST}
	${RESOURCEFILES_LIST}
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
TARGET_LINK_LIBRARIES(gateway ws2_32.lib odbc32.lib odbccp32.lib luascript.lib libiconv.lib)
else()
TARGET_LINK_LIBRARIES(gateway pthread odbc iconv luascript)
  if(USE_32BITS)
    SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=i386 -mtune=i686")
    SET(CMAKE_CXX_FLAGS "-Wall -ggdb -pipe -march=i386 -mtune=i686")
  else()
    SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=x86-64 -mtune=i686")
    SET(CMAKE_CXX_FLAGS "-Wall -ggdb -pipe -march=x86-64 -mtune=i686")
  endif(USE_32BITS)
endif(WIN32)
