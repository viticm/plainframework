SET(TF_CURRENT_BINARY_PATH_BIN "../../bin")
ADD_DEFINITIONS(-D_CENTER)
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
  "../../../pf/dependencies/dll" #linux
)

SET(EXECUTABLE_OUTPUT_PATH ${TF_CURRENT_BINARY_PATH_BIN})


INCLUDE_DIRECTORIES("../../../pf/core/include",
                    "../include",
                    "../../common/include",
                    "../../../pf/dependencies/iconv/include",
                    "../../../pf/dependencies/lua/src",
                    "/usr/local/unixODBC/include")

##update_begin




SET (SOURCEFILES_MESSAGE_HANDLER_SERVERSERVER_LIST
	../src/message/handler/serverserver/connect.cc
)

SET (SOURCEFILES_MESSAGE_HANDLER_LIST
)

SET (SOURCEFILES_MESSAGE_LIST
)

SET (SOURCEFILES_ENGINE_LIST
	../src/engine/system.cc
)

SET (SOURCEFILES_LIST
	../src/main.cc
)

SET (HEADERFILES_APPLICATION_LIST
	../include/application/config.h
)

SET (HEADERFILES_ENGINE_LIST
	../include/engine/config.h
	../include/engine/system.h
)

SET (HEADERFILES_LIST
	../include/main.h
)

SET (RESOURCEFILES_LIST
)

SET (COMMON_SRC_NET_PACKET_SERVERSERVER_LIST
	../../common/src/net/packet/serverserver/connect.cc
)

SET (COMMON_SRC_NET_PACKET_LIST
)

SET (COMMON_SRC_NET_LIST
	../../common/src/net/packetfactory.cc
)

SET (COMMON_SRC_SCRIPT_LUA_LIST
	../../common/src/script/lua/export.cc
	../../common/src/script/lua/file.cc
	../../common/src/script/lua/log.cc
)

SET (COMMON_SRC_SCRIPT_LIST
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
	../../common/include/common/define/file.h
	../../common/include/common/define/log.h
	../../common/include/common/define/macros.h
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

SET (COMMON_INCLUDE_SCRIPT_LUA_LIST
	../../common/include/common/script/lua/config.h
	../../common/include/common/script/lua/export.h
	../../common/include/common/script/lua/file.h
	../../common/include/common/script/lua/log.h
)

SET (COMMON_INCLUDE_SCRIPT_LIST
	../../common/include/common/script/config.h
)

SET (COMMON_INCLUDE_LIST
	../../common/include/common/config.h
	../../common/include/common/setting.h
)

SET (COMMON_LIST
)



if (WIN32)
	source_group(SourceFiles\\message\\handler\\serverserver FILES ${SOURCEFILES_MESSAGE_HANDLER_SERVERSERVER_LIST})
	source_group(SourceFiles\\message\\handler FILES ${SOURCEFILES_MESSAGE_HANDLER_LIST})
	source_group(SourceFiles\\message FILES ${SOURCEFILES_MESSAGE_LIST})
	source_group(SourceFiles\\engine FILES ${SOURCEFILES_ENGINE_LIST})
	source_group(SourceFiles FILES ${SOURCEFILES_LIST})
	source_group(HeaderFiles\\application FILES ${HEADERFILES_APPLICATION_LIST})
	source_group(HeaderFiles\\engine FILES ${HEADERFILES_ENGINE_LIST})
	source_group(HeaderFiles FILES ${HEADERFILES_LIST})
	source_group(ResourceFiles FILES ${RESOURCEFILES_LIST})
	source_group(common\\src\\net\\packet\\serverserver FILES ${COMMON_SRC_NET_PACKET_SERVERSERVER_LIST})
	source_group(common\\src\\net\\packet FILES ${COMMON_SRC_NET_PACKET_LIST})
	source_group(common\\src\\net FILES ${COMMON_SRC_NET_LIST})
	source_group(common\\src\\script\\lua FILES ${COMMON_SRC_SCRIPT_LUA_LIST})
	source_group(common\\src\\script FILES ${COMMON_SRC_SCRIPT_LIST})
	source_group(common\\src FILES ${COMMON_SRC_LIST})
	source_group(common\\include\\define\\net\\packet\\id FILES ${COMMON_INCLUDE_DEFINE_NET_PACKET_ID_LIST})
	source_group(common\\include\\define\\net\\packet FILES ${COMMON_INCLUDE_DEFINE_NET_PACKET_LIST})
	source_group(common\\include\\define\\net FILES ${COMMON_INCLUDE_DEFINE_NET_LIST})
	source_group(common\\include\\define FILES ${COMMON_INCLUDE_DEFINE_LIST})
	source_group(common\\include\\net\\packet\\serverserver FILES ${COMMON_INCLUDE_NET_PACKET_SERVERSERVER_LIST})
	source_group(common\\include\\net\\packet FILES ${COMMON_INCLUDE_NET_PACKET_LIST})
	source_group(common\\include\\net FILES ${COMMON_INCLUDE_NET_LIST})
	source_group(common\\include\\script\\lua FILES ${COMMON_INCLUDE_SCRIPT_LUA_LIST})
	source_group(common\\include\\script FILES ${COMMON_INCLUDE_SCRIPT_LIST})
	source_group(common\\include FILES ${COMMON_INCLUDE_LIST})
	source_group(common FILES ${COMMON_LIST})
endif()


ADD_EXECUTABLE(center
	${SOURCEFILES_MESSAGE_HANDLER_SERVERSERVER_LIST}
	${SOURCEFILES_MESSAGE_HANDLER_LIST}
	${SOURCEFILES_MESSAGE_LIST}
	${SOURCEFILES_ENGINE_LIST}
	${SOURCEFILES_LIST}
	${HEADERFILES_APPLICATION_LIST}
	${HEADERFILES_ENGINE_LIST}
	${HEADERFILES_LIST}
	${RESOURCEFILES_LIST}
	${COMMON_SRC_NET_PACKET_SERVERSERVER_LIST}
	${COMMON_SRC_NET_PACKET_LIST}
	${COMMON_SRC_NET_LIST}
	${COMMON_SRC_SCRIPT_LUA_LIST}
	${COMMON_SRC_SCRIPT_LIST}
	${COMMON_SRC_LIST}
	${COMMON_INCLUDE_DEFINE_NET_PACKET_ID_LIST}
	${COMMON_INCLUDE_DEFINE_NET_PACKET_LIST}
	${COMMON_INCLUDE_DEFINE_NET_LIST}
	${COMMON_INCLUDE_DEFINE_LIST}
	${COMMON_INCLUDE_NET_PACKET_SERVERSERVER_LIST}
	${COMMON_INCLUDE_NET_PACKET_LIST}
	${COMMON_INCLUDE_NET_LIST}
	${COMMON_INCLUDE_SCRIPT_LUA_LIST}
	${COMMON_INCLUDE_SCRIPT_LIST}
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
TARGET_LINK_LIBRARIES(center ws2_32.lib odbc32.lib odbccp32.lib pf_core.lib iconv.lib lua.lib)
else()
TARGET_LINK_LIBRARIES(center pthread odbc pf_core lua iconv)
  if(USE_32BITS)
    SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=i386 -mtune=i686")
    SET(CMAKE_CXX_FLAGS "-Wall -ggdb -pipe -march=i386 -mtune=i686")
  else()
    SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=x86-64 -mtune=i686")
    SET(CMAKE_CXX_FLAGS "-Wall -ggdb -pipe -march=x86-64 -mtune=i686")
  endif(USE_32BITS)
endif(WIN32)
