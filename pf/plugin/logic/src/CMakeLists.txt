# This file generate by plain framework admin tool, date: 2015-05-09 13:20:51
SET(LIBRARY_OUTPUT_PATH ./../../../dll/)


SET(USE_32BITS 0)

IF(CMAKE_SYSTEM MATCHES Linux)
  ADD_DEFINITIONS(-D_REENTRANT)
  ADD_DEFINITIONS(-DDONT_TD_VOID)
  ADD_DEFINITIONS(-D_PF_NET_EPOLL)
ELSE(CMAKE_SYSTEM MATCHES Linux)
ENDIF(CMAKE_SYSTEM MATCHES Linux)

LINK_DIRECTORIES(
  "./../../../dependencies/dll/"
  "./../../../core/dll/"
)

INCLUDE_DIRECTORIES(
  "../include/"
  "../../../dependencies/lua/include/"
  "../../../dependencies/iconv/include/"
  "../../../plugin/structure/include/"
  "../../../core/include/"
)


SET (INCLUDE_LOGIC_LIST
  ../include/logic/api.h
  ../include/logic/controllerinterface.h
  ../include/logic/interface.h
  ../include/logic/managerinterface.h
  ../include/logic/moduleinterface.h
  ../include/logic/systeminterface.h
  ../include/logic/viewinterface.h
)

SET (SRC_LIST
  ../src/manager.cc
)


ADD_LIBRARY(plugin_logic SHARED
  ${SRC_LIST}
)

if(USE_32BITS)
  SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=i386 -mtune=i686")
  SET(CMAKE_CXX_FLAGS "-Wall -ggdb -pipe -march=i386 -mtune=i686")
else()
  SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=x86-64 -mtune=i686")
  SET(CMAKE_CXX_FLAGS "-Wall -ggdb -pipe -march=x86-64 -mtune=i686")
endif(USE_32BITS)

if (WIN32)
TARGET_LINK_LIBRARIES(plugin_logic ws2_32.lib odbc32.lib odbccp32.lib pf_core.lib lua.lib iconv.lib)
else()
TARGET_LINK_LIBRARIES(plugin_logic odbc pf_core lua iconv)
  if(USE_32BITS)
    SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=i386 -mtune=i686")
    SET(CMAKE_CXX_FLAGS "-Wall -ggdb -pipe -march=i386 -mtune=i686")
  else()
    SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=x86-64 -mtune=i686")
    SET(CMAKE_CXX_FLAGS "-Wall -ggdb -pipe -march=x86-64 -mtune=i686")
  endif(USE_32BITS)
endif(WIN32)
