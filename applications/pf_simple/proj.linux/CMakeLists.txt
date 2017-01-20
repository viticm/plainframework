# This file generate by plain framework admin tool, date: 2017-01-19 00:12:41
SET(EXECUTABLE_OUTPUT_PATH ../../bin)

ADD_DEFINITIONS(-DPF_BUILD_AS_DLL)
ADD_DEFINITIONS(-DPF_OPEN_ICONV)
ADD_DEFINITIONS(-DPF_OPEN_EPOLL)
ADD_DEFINITIONS(-DPF_OPEN_ODBC)
ADD_DEFINITIONS(-DPF_OPEN_LUA)
SET(USE_32BITS 0)

IF(CMAKE_SYSTEM MATCHES Linux)
  ADD_DEFINITIONS(-D_REENTRANT)
  ADD_DEFINITIONS(-DDONT_TD_VOID)
ELSE(CMAKE_SYSTEM MATCHES Linux)
ENDIF(CMAKE_SYSTEM MATCHES Linux)

LINK_DIRECTORIES(
  "./"
  "./../../../pf/core/dll/"
  "./../../../pf/dependencies/dll/"
)

INCLUDE_DIRECTORIES(
  "../../../pf/core/include/"
  "../include/"
  "../../../pf/dependencies/lua/5.3.0/include/"
  "/usr/local/unixODBC/include/"
)


SET (INCLUDE_LIST
  ../include/_config.h
  ../include/main.h
  ../include/net.h
)

SET (INCLUDE_PACKET_LIST
  ../include/packet/config.h
  ../include/packet/helloworld.h
  ../include/packet/sayhello.h
)

SET (SRC_LIST
  ../src/main.cc
  ../src/net.cc
)

SET (SRC_PACKET_LIST
  ../src/packet/helloworld.cc
  ../src/packet/sayhello.cc
)


ADD_EXECUTABLE(pf_simple
  ${SRC_LIST}
  ${SRC_PACKET_LIST}
)

if (WIN32)
TARGET_LINK_LIBRARIES(pf_simple ws2_32.lib pf_core.lib lua.lib iconv.lib)
else()
TARGET_LINK_LIBRARIES(pf_simple pf_core lua iconv)
  if(USE_32BITS)
    SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=i386 -mtune=i686")
    SET(CMAKE_CXX_FLAGS "-std=c++11 -Wall -ggdb -pipe -march=i386 -mtune=i686")
  else()
    SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=x86-64 -mtune=i686")
    SET(CMAKE_CXX_FLAGS "-std=c++11 -Wall -ggdb -pipe -march=x86-64 -mtune=i686")
  endif(USE_32BITS)
endif(WIN32)
