SET(TF_CURRENT_BINARY_PATH_BIN "../../bin")
ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE)
ADD_DEFINITIONS(-D_PF_SIMPLE)

IF(CMAKE_SYSTEM MATCHES Linux)
  ADD_DEFINITIONS(-D_REENTRANT)
  ADD_DEFINITIONS(-DDONT_TD_VOID)
  ADD_DEFINITIONS(-D_PF_NET_EPOLL)
ELSE(CMAKE_SYSTEM MATCHES Linux)
ENDIF(CMAKE_SYSTEM MATCHES Linux)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})
LINK_DIRECTORIES(
  "./" #run dir
  "../../../pf/dependencies/lib" #linux
)

SET(EXECUTABLE_OUTPUT_PATH ${TF_CURRENT_BINARY_PATH_BIN})


INCLUDE_DIRECTORIES("../../../pf/core/include",
                    "../include",
                    "../../../pf/dependencies/iconv/include",
                    "../../../pf/dependencies/lua/src",
                    "/usr/local/unixODBC/include")

##update_begin




SET (SOURCEFILES_PF_BASE_HASHMAP_LIST
	../../../pf/core/src/base/hashmap/integer.cc
)

SET (SOURCEFILES_PF_BASE_LIST
	../../../pf/core/src/base/io.cc
	../../../pf/core/src/base/log.cc
	../../../pf/core/src/base/md5.cc
	../../../pf/core/src/base/string.cc
	../../../pf/core/src/base/time_manager.cc
	../../../pf/core/src/base/util.cc
)

SET (SOURCEFILES_PF_SYS_MEMORY_LIST
	../../../pf/core/src/sys/memory/dynamic_allocator.cc
	../../../pf/core/src/sys/memory/share.cc
	../../../pf/core/src/sys/memory/static_allocator.cc
)

SET (SOURCEFILES_PF_SYS_LIST
	../../../pf/core/src/sys/assert.cc
	../../../pf/core/src/sys/info.cc
	../../../pf/core/src/sys/minidump.cc
	../../../pf/core/src/sys/process.cc
	../../../pf/core/src/sys/thread.cc
	../../../pf/core/src/sys/util.cc
)

SET (SOURCEFILES_PF_FILE_LIST
	../../../pf/core/src/file/api.cc
	../../../pf/core/src/file/database.cc
	../../../pf/core/src/file/ini.cc
)

SET (SOURCEFILES_PF_NET_PACKET_LIST
	../../../pf/core/src/net/packet/base.cc
	../../../pf/core/src/net/packet/factorymanager.cc
)

SET (SOURCEFILES_PF_NET_SOCKET_LIST
	../../../pf/core/src/net/socket/api.cc
	../../../pf/core/src/net/socket/base.cc
	../../../pf/core/src/net/socket/encode.cc
	../../../pf/core/src/net/socket/inputstream.cc
	../../../pf/core/src/net/socket/outputstream.cc
	../../../pf/core/src/net/socket/server.cc
	../../../pf/core/src/net/socket/stream.cc
)

SET (SOURCEFILES_PF_NET_CONNECTION_MANAGER_LIST
	../../../pf/core/src/net/connection/manager/base.cc
	../../../pf/core/src/net/connection/manager/epoll.cc
	../../../pf/core/src/net/connection/manager/iocp.cc
	../../../pf/core/src/net/connection/manager/select.cc
)

SET (SOURCEFILES_PF_NET_CONNECTION_LIST
	../../../pf/core/src/net/connection/base.cc
	../../../pf/core/src/net/connection/pool.cc
	../../../pf/core/src/net/connection/server.cc
)

SET (SOURCEFILES_PF_NET_LIST
	../../../pf/core/src/net/manager.cc
)

SET (SOURCEFILES_PF_ENGINE_THREAD_LIST
	../../../pf/core/src/engine/thread/db.cc
	../../../pf/core/src/engine/thread/net.cc
	../../../pf/core/src/engine/thread/performance.cc
	../../../pf/core/src/engine/thread/script.cc
)

SET (SOURCEFILES_PF_ENGINE_LIST
	../../../pf/core/src/engine/kernel.cc
)

SET (SOURCEFILES_PF_DB_ODBC_LIST
	../../../pf/core/src/db/odbc/interface.cc
	../../../pf/core/src/db/odbc/system.cc
)

SET (SOURCEFILES_PF_DB_LIST
	../../../pf/core/src/db/manager.cc
)

SET (SOURCEFILES_PF_SCRIPT_CACHE_LIST
	../../../pf/core/src/script/cache/base.cc
	../../../pf/core/src/script/cache/manager.cc
)

SET (SOURCEFILES_PF_SCRIPT_LUA_LIST
	../../../pf/core/src/script/lua/filebridge.cc
	../../../pf/core/src/script/lua/interface.cc
	../../../pf/core/src/script/lua/lunar.cc
	../../../pf/core/src/script/lua/stack.cc
	../../../pf/core/src/script/lua/system.cc
	../../../pf/core/src/script/lua/vm.cc
)

SET (SOURCEFILES_PF_SCRIPT_LIST
)

SET (SOURCEFILES_PF_PERFORMANCE_LIST
	../../../pf/core/src/performance/eyes.cc
)

SET (SOURCEFILES_PF_UTIL_COMPRESSOR_LIST
	../../../pf/core/src/util/compressor/mini.cc
	../../../pf/core/src/util/compressor/minilzo.c
)

SET (SOURCEFILES_PF_UTIL_LIST
)

SET (SOURCEFILES_PF_APPLICATION_EXTEND_LIST
	../../../pf/core/src/application/extend/log.cc
	../../../pf/core/src/application/extend/net_packet_factorymanager.cc
)

SET (SOURCEFILES_PF_APPLICATION_LIST
	../../../pf/core/src/application/setting.cc
)

SET (SOURCEFILES_PF_LIST
)

SET (SOURCEFILES_LIST
	../src/main.cc
)

SET (HEADERFILES_PF_BASE_HASHMAP_LIST
	../../../pf/core/include/pf/base/hashmap/config.h
	../../../pf/core/include/pf/base/hashmap/integer.h
	../../../pf/core/include/pf/base/hashmap/template.h
)

SET (HEADERFILES_PF_BASE_LIST
	../../../pf/core/include/pf/base/base64.hpp
	../../../pf/core/include/pf/base/config.h
	../../../pf/core/include/pf/base/io.h
	../../../pf/core/include/pf/base/log.h
	../../../pf/core/include/pf/base/md5.h
	../../../pf/core/include/pf/base/singleton.h
	../../../pf/core/include/pf/base/string.h
	../../../pf/core/include/pf/base/time_manager.h
	../../../pf/core/include/pf/base/type.h
	../../../pf/core/include/pf/base/util.h
)

SET (HEADERFILES_PF_DB_ODBC_LIST
	../../../pf/core/include/pf/db/odbc/config.h
	../../../pf/core/include/pf/db/odbc/interface.h
	../../../pf/core/include/pf/db/odbc/system.h
)

SET (HEADERFILES_PF_DB_LIST
	../../../pf/core/include/pf/db/config.h
	../../../pf/core/include/pf/db/manager.h
)

SET (HEADERFILES_PF_FILE_LIST
	../../../pf/core/include/pf/file/api.h
	../../../pf/core/include/pf/file/config.h
	../../../pf/core/include/pf/file/database.h
	../../../pf/core/include/pf/file/ini.h
)

SET (HEADERFILES_PF_NET_SOCKET_LIST
	../../../pf/core/include/pf/net/socket/api.h
	../../../pf/core/include/pf/net/socket/base.h
	../../../pf/core/include/pf/net/socket/config.h
	../../../pf/core/include/pf/net/socket/encode.h
	../../../pf/core/include/pf/net/socket/extend.inl
	../../../pf/core/include/pf/net/socket/inputstream.h
	../../../pf/core/include/pf/net/socket/outputstream.h
	../../../pf/core/include/pf/net/socket/server.h
	../../../pf/core/include/pf/net/socket/stream.h
)

SET (HEADERFILES_PF_NET_PACKET_LIST
	../../../pf/core/include/pf/net/packet/base.h
	../../../pf/core/include/pf/net/packet/config.h
	../../../pf/core/include/pf/net/packet/factory.h
	../../../pf/core/include/pf/net/packet/factorymanager.h
)

SET (HEADERFILES_PF_NET_CONNECTION_MANAGER_LIST
	../../../pf/core/include/pf/net/connection/manager/base.h
	../../../pf/core/include/pf/net/connection/manager/config.h
	../../../pf/core/include/pf/net/connection/manager/epoll.h
	../../../pf/core/include/pf/net/connection/manager/iocp.h
	../../../pf/core/include/pf/net/connection/manager/select.h
)

SET (HEADERFILES_PF_NET_CONNECTION_LIST
	../../../pf/core/include/pf/net/connection/base.h
	../../../pf/core/include/pf/net/connection/config.h
	../../../pf/core/include/pf/net/connection/pool.h
	../../../pf/core/include/pf/net/connection/server.h
)

SET (HEADERFILES_PF_NET_LIST
	../../../pf/core/include/pf/net/config.h
	../../../pf/core/include/pf/net/manager.h
)

SET (HEADERFILES_PF_SCRIPT_CACHE_LIST
	../../../pf/core/include/pf/script/cache/base.h
	../../../pf/core/include/pf/script/cache/config.h
	../../../pf/core/include/pf/script/cache/manager.h
)

SET (HEADERFILES_PF_SCRIPT_LUA_LIST
	../../../pf/core/include/pf/script/lua/config.h
	../../../pf/core/include/pf/script/lua/filebridge.h
	../../../pf/core/include/pf/script/lua/interface.h
	../../../pf/core/include/pf/script/lua/lunar.h
	../../../pf/core/include/pf/script/lua/stack.h
	../../../pf/core/include/pf/script/lua/system.h
	../../../pf/core/include/pf/script/lua/vm.h
)

SET (HEADERFILES_PF_SCRIPT_LIST
	../../../pf/core/include/pf/script/config.h
)

SET (HEADERFILES_PF_ENGINE_THREAD_LIST
	../../../pf/core/include/pf/engine/thread/config.h
	../../../pf/core/include/pf/engine/thread/db.h
	../../../pf/core/include/pf/engine/thread/net.h
	../../../pf/core/include/pf/engine/thread/performance.h
	../../../pf/core/include/pf/engine/thread/script.h
)

SET (HEADERFILES_PF_ENGINE_LIST
	../../../pf/core/include/pf/engine/config.h
	../../../pf/core/include/pf/engine/kernel.h
)

SET (HEADERFILES_PF_SYS_MEMORY_LIST
	../../../pf/core/include/pf/sys/memory/config.h
	../../../pf/core/include/pf/sys/memory/dynamic_allocator.h
	../../../pf/core/include/pf/sys/memory/share.h
	../../../pf/core/include/pf/sys/memory/static_allocator.h
)

SET (HEADERFILES_PF_SYS_LIST
	../../../pf/core/include/pf/sys/assert.h
	../../../pf/core/include/pf/sys/config.h
	../../../pf/core/include/pf/sys/info.h
	../../../pf/core/include/pf/sys/minidump.h
	../../../pf/core/include/pf/sys/process.h
	../../../pf/core/include/pf/sys/thread.h
	../../../pf/core/include/pf/sys/util.h
)

SET (HEADERFILES_PF_PERFORMANCE_LIST
	../../../pf/core/include/pf/performance/config.h
	../../../pf/core/include/pf/performance/eyes.h
)

SET (HEADERFILES_PF_UTIL_COMPRESSOR_LIST
	../../../pf/core/include/pf/util/compressor/config.h
	../../../pf/core/include/pf/util/compressor/lzoconf.h
	../../../pf/core/include/pf/util/compressor/lzodefs.h
	../../../pf/core/include/pf/util/compressor/mini.h
	../../../pf/core/include/pf/util/compressor/minilzo.h
)

SET (HEADERFILES_PF_UTIL_LIST
	../../../pf/core/include/pf/util/config.h
)

SET (HEADERFILES_PF_APPLICATION_DEFINE_NET_PACKET_ID_LIST
	../../../pf/core/include/pf/application/define/net/packet/id/all.h
	../../../pf/core/include/pf/application/define/net/packet/id/clientlogin.h
	../../../pf/core/include/pf/application/define/net/packet/id/clientserver.h
	../../../pf/core/include/pf/application/define/net/packet/id/config.h
	../../../pf/core/include/pf/application/define/net/packet/id/gatewaylogin.h
	../../../pf/core/include/pf/application/define/net/packet/id/logincenter.h
	../../../pf/core/include/pf/application/define/net/packet/id/servercenter.h
	../../../pf/core/include/pf/application/define/net/packet/id/serverserver.h
)

SET (HEADERFILES_PF_APPLICATION_DEFINE_NET_PACKET_LIST
	../../../pf/core/include/pf/application/define/net/packet/config.h
)

SET (HEADERFILES_PF_APPLICATION_DEFINE_NET_LIST
	../../../pf/core/include/pf/application/define/net/config.h
)

SET (HEADERFILES_PF_APPLICATION_DEFINE_LIST
	../../../pf/core/include/pf/application/define/all.h
	../../../pf/core/include/pf/application/define/config.h
	../../../pf/core/include/pf/application/define/file.h
	../../../pf/core/include/pf/application/define/log.h
	../../../pf/core/include/pf/application/define/macros.h
)

SET (HEADERFILES_PF_APPLICATION_EXTEND_LIST
	../../../pf/core/include/pf/application/extend/config.h
	../../../pf/core/include/pf/application/extend/define.h
	../../../pf/core/include/pf/application/extend/log.h
	../../../pf/core/include/pf/application/extend/net_packet_factorymanager.h
)

SET (HEADERFILES_PF_APPLICATION_LIST
	../../../pf/core/include/pf/application/config.h
	../../../pf/core/include/pf/application/setting.h
)

SET (HEADERFILES_PF_LIST
)

SET (HEADERFILES_LIST
)

SET (RESOURCEFILES_LIST
)



if (WIN32)
	source_group(SourceFiles\\pf\\base\\hashmap FILES ${SOURCEFILES_PF_BASE_HASHMAP_LIST})
	source_group(SourceFiles\\pf\\base FILES ${SOURCEFILES_PF_BASE_LIST})
	source_group(SourceFiles\\pf\\sys\\memory FILES ${SOURCEFILES_PF_SYS_MEMORY_LIST})
	source_group(SourceFiles\\pf\\sys FILES ${SOURCEFILES_PF_SYS_LIST})
	source_group(SourceFiles\\pf\\file FILES ${SOURCEFILES_PF_FILE_LIST})
	source_group(SourceFiles\\pf\\net\\packet FILES ${SOURCEFILES_PF_NET_PACKET_LIST})
	source_group(SourceFiles\\pf\\net\\socket FILES ${SOURCEFILES_PF_NET_SOCKET_LIST})
	source_group(SourceFiles\\pf\\net\\connection\\manager FILES ${SOURCEFILES_PF_NET_CONNECTION_MANAGER_LIST})
	source_group(SourceFiles\\pf\\net\\connection FILES ${SOURCEFILES_PF_NET_CONNECTION_LIST})
	source_group(SourceFiles\\pf\\net FILES ${SOURCEFILES_PF_NET_LIST})
	source_group(SourceFiles\\pf\\engine\\thread FILES ${SOURCEFILES_PF_ENGINE_THREAD_LIST})
	source_group(SourceFiles\\pf\\engine FILES ${SOURCEFILES_PF_ENGINE_LIST})
	source_group(SourceFiles\\pf\\db\\odbc FILES ${SOURCEFILES_PF_DB_ODBC_LIST})
	source_group(SourceFiles\\pf\\db FILES ${SOURCEFILES_PF_DB_LIST})
	source_group(SourceFiles\\pf\\script\\cache FILES ${SOURCEFILES_PF_SCRIPT_CACHE_LIST})
	source_group(SourceFiles\\pf\\script\\lua FILES ${SOURCEFILES_PF_SCRIPT_LUA_LIST})
	source_group(SourceFiles\\pf\\script FILES ${SOURCEFILES_PF_SCRIPT_LIST})
	source_group(SourceFiles\\pf\\performance FILES ${SOURCEFILES_PF_PERFORMANCE_LIST})
	source_group(SourceFiles\\pf\\util\\compressor FILES ${SOURCEFILES_PF_UTIL_COMPRESSOR_LIST})
	source_group(SourceFiles\\pf\\util FILES ${SOURCEFILES_PF_UTIL_LIST})
	source_group(SourceFiles\\pf\\application\\extend FILES ${SOURCEFILES_PF_APPLICATION_EXTEND_LIST})
	source_group(SourceFiles\\pf\\application FILES ${SOURCEFILES_PF_APPLICATION_LIST})
	source_group(SourceFiles\\pf FILES ${SOURCEFILES_PF_LIST})
	source_group(SourceFiles FILES ${SOURCEFILES_LIST})
	source_group(HeaderFiles\\pf\\base\\hashmap FILES ${HEADERFILES_PF_BASE_HASHMAP_LIST})
	source_group(HeaderFiles\\pf\\base FILES ${HEADERFILES_PF_BASE_LIST})
	source_group(HeaderFiles\\pf\\db\\odbc FILES ${HEADERFILES_PF_DB_ODBC_LIST})
	source_group(HeaderFiles\\pf\\db FILES ${HEADERFILES_PF_DB_LIST})
	source_group(HeaderFiles\\pf\\file FILES ${HEADERFILES_PF_FILE_LIST})
	source_group(HeaderFiles\\pf\\net\\socket FILES ${HEADERFILES_PF_NET_SOCKET_LIST})
	source_group(HeaderFiles\\pf\\net\\packet FILES ${HEADERFILES_PF_NET_PACKET_LIST})
	source_group(HeaderFiles\\pf\\net\\connection\\manager FILES ${HEADERFILES_PF_NET_CONNECTION_MANAGER_LIST})
	source_group(HeaderFiles\\pf\\net\\connection FILES ${HEADERFILES_PF_NET_CONNECTION_LIST})
	source_group(HeaderFiles\\pf\\net FILES ${HEADERFILES_PF_NET_LIST})
	source_group(HeaderFiles\\pf\\script\\cache FILES ${HEADERFILES_PF_SCRIPT_CACHE_LIST})
	source_group(HeaderFiles\\pf\\script\\lua FILES ${HEADERFILES_PF_SCRIPT_LUA_LIST})
	source_group(HeaderFiles\\pf\\script FILES ${HEADERFILES_PF_SCRIPT_LIST})
	source_group(HeaderFiles\\pf\\engine\\thread FILES ${HEADERFILES_PF_ENGINE_THREAD_LIST})
	source_group(HeaderFiles\\pf\\engine FILES ${HEADERFILES_PF_ENGINE_LIST})
	source_group(HeaderFiles\\pf\\sys\\memory FILES ${HEADERFILES_PF_SYS_MEMORY_LIST})
	source_group(HeaderFiles\\pf\\sys FILES ${HEADERFILES_PF_SYS_LIST})
	source_group(HeaderFiles\\pf\\performance FILES ${HEADERFILES_PF_PERFORMANCE_LIST})
	source_group(HeaderFiles\\pf\\util\\compressor FILES ${HEADERFILES_PF_UTIL_COMPRESSOR_LIST})
	source_group(HeaderFiles\\pf\\util FILES ${HEADERFILES_PF_UTIL_LIST})
	source_group(HeaderFiles\\pf\\application\\define\\net\\packet\\id FILES ${HEADERFILES_PF_APPLICATION_DEFINE_NET_PACKET_ID_LIST})
	source_group(HeaderFiles\\pf\\application\\define\\net\\packet FILES ${HEADERFILES_PF_APPLICATION_DEFINE_NET_PACKET_LIST})
	source_group(HeaderFiles\\pf\\application\\define\\net FILES ${HEADERFILES_PF_APPLICATION_DEFINE_NET_LIST})
	source_group(HeaderFiles\\pf\\application\\define FILES ${HEADERFILES_PF_APPLICATION_DEFINE_LIST})
	source_group(HeaderFiles\\pf\\application\\extend FILES ${HEADERFILES_PF_APPLICATION_EXTEND_LIST})
	source_group(HeaderFiles\\pf\\application FILES ${HEADERFILES_PF_APPLICATION_LIST})
	source_group(HeaderFiles\\pf FILES ${HEADERFILES_PF_LIST})
	source_group(HeaderFiles FILES ${HEADERFILES_LIST})
	source_group(ResourceFiles FILES ${RESOURCEFILES_LIST})
endif()


ADD_EXECUTABLE(pf_simple
	${SOURCEFILES_PF_BASE_HASHMAP_LIST}
	${SOURCEFILES_PF_BASE_LIST}
	${SOURCEFILES_PF_SYS_MEMORY_LIST}
	${SOURCEFILES_PF_SYS_LIST}
	${SOURCEFILES_PF_FILE_LIST}
	${SOURCEFILES_PF_NET_PACKET_LIST}
	${SOURCEFILES_PF_NET_SOCKET_LIST}
	${SOURCEFILES_PF_NET_CONNECTION_MANAGER_LIST}
	${SOURCEFILES_PF_NET_CONNECTION_LIST}
	${SOURCEFILES_PF_NET_LIST}
	${SOURCEFILES_PF_ENGINE_THREAD_LIST}
	${SOURCEFILES_PF_ENGINE_LIST}
	${SOURCEFILES_PF_DB_ODBC_LIST}
	${SOURCEFILES_PF_DB_LIST}
	${SOURCEFILES_PF_SCRIPT_CACHE_LIST}
	${SOURCEFILES_PF_SCRIPT_LUA_LIST}
	${SOURCEFILES_PF_SCRIPT_LIST}
	${SOURCEFILES_PF_PERFORMANCE_LIST}
	${SOURCEFILES_PF_UTIL_COMPRESSOR_LIST}
	${SOURCEFILES_PF_UTIL_LIST}
	${SOURCEFILES_PF_APPLICATION_EXTEND_LIST}
	${SOURCEFILES_PF_APPLICATION_LIST}
	${SOURCEFILES_PF_LIST}
	${SOURCEFILES_LIST}
	${HEADERFILES_PF_BASE_HASHMAP_LIST}
	${HEADERFILES_PF_BASE_LIST}
	${HEADERFILES_PF_DB_ODBC_LIST}
	${HEADERFILES_PF_DB_LIST}
	${HEADERFILES_PF_FILE_LIST}
	${HEADERFILES_PF_NET_SOCKET_LIST}
	${HEADERFILES_PF_NET_PACKET_LIST}
	${HEADERFILES_PF_NET_CONNECTION_MANAGER_LIST}
	${HEADERFILES_PF_NET_CONNECTION_LIST}
	${HEADERFILES_PF_NET_LIST}
	${HEADERFILES_PF_SCRIPT_CACHE_LIST}
	${HEADERFILES_PF_SCRIPT_LUA_LIST}
	${HEADERFILES_PF_SCRIPT_LIST}
	${HEADERFILES_PF_ENGINE_THREAD_LIST}
	${HEADERFILES_PF_ENGINE_LIST}
	${HEADERFILES_PF_SYS_MEMORY_LIST}
	${HEADERFILES_PF_SYS_LIST}
	${HEADERFILES_PF_PERFORMANCE_LIST}
	${HEADERFILES_PF_UTIL_COMPRESSOR_LIST}
	${HEADERFILES_PF_UTIL_LIST}
	${HEADERFILES_PF_APPLICATION_DEFINE_NET_PACKET_ID_LIST}
	${HEADERFILES_PF_APPLICATION_DEFINE_NET_PACKET_LIST}
	${HEADERFILES_PF_APPLICATION_DEFINE_NET_LIST}
	${HEADERFILES_PF_APPLICATION_DEFINE_LIST}
	${HEADERFILES_PF_APPLICATION_EXTEND_LIST}
	${HEADERFILES_PF_APPLICATION_LIST}
	${HEADERFILES_PF_LIST}
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
TARGET_LINK_LIBRARIES(pf_simple ws2_32.lib odbc32.lib odbccp32.lib luascript.lib iconv.lib)
else()
TARGET_LINK_LIBRARIES(pf_simple pthread odbc iconv luascript)
  if(USE_32BITS)
    SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=i386 -mtune=i686")
    SET(CMAKE_CXX_FLAGS "-Wall -ggdb -pipe -march=i386 -mtune=i686")
  else()
    SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=x86-64 -mtune=i686")
    SET(CMAKE_CXX_FLAGS "-Wall -ggdb -pipe -march=x86-64 -mtune=i686")
  endif(USE_32BITS)
endif(WIN32)
