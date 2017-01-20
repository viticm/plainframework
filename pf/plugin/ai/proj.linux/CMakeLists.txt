# This file generate by plain framework admin tool, date: 2015-05-11 19:17:28
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
  "../../../dependencies/iconv/include/"
  "../../../plugin/structure/include/"
  "../../../core/include/"
)


SET (INCLUDE_AI_LIST
  ../include/ai/action.h
  ../include/ai/base.h
  ../include/ai/config.h
  ../include/ai/controller.h
  ../include/ai/define.h
  ../include/ai/event.h
  ../include/ai/logic.h
  ../include/ai/manager.h
  ../include/ai/state.h
  ../include/ai/template.h
  ../include/ai/vm.h
)

SET (SRC_LIST
  ../src/action.cc
  ../src/base.cc
  ../src/controller.cc
  ../src/define.cc
  ../src/event.cc
  ../src/logic.cc
  ../src/manager.cc
  ../src/state.cc
  ../src/template.cc
  ../src/vm.cc
)


ADD_LIBRARY(plugin_ai SHARED
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
TARGET_LINK_LIBRARIES(plugin_ai ws2_32.lib pf_core.lib lua.lib iconv.lib)
else()
TARGET_LINK_LIBRARIES(plugin_ai pf_core lua iconv)
  if(USE_32BITS)
    SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=i386 -mtune=i686")
    SET(CMAKE_CXX_FLAGS "-Wall -ggdb -pipe -march=i386 -mtune=i686")
  else()
    SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=x86-64 -mtune=i686")
    SET(CMAKE_CXX_FLAGS "-Wall -ggdb -pipe -march=x86-64 -mtune=i686")
  endif(USE_32BITS)
endif(WIN32)
