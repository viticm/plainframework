# This file generate by plain framework admin tool, date: 2015-05-11 19:16:48
SET(LIBRARY_OUTPUT_PATH ./../../../dll/)

ADD_DEFINITIONS(-DPF_PLUGIN_CORE)
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
  "../../../core/include/"
)


SET (INCLUDE_STRUCTURE_LIST
  ../include/structure/config.h
  ../include/structure/define.h
  ../include/structure/math.h
  ../include/structure/object.h
  ../include/structure/objectlist.h
)

SET (INCLUDE_STRUCTURE_MANAGER_LIST
  ../include/structure/manager/config.h
  ../include/structure/manager/object.h
  ../include/structure/manager/objectsingle.h
)

SET (SRC_LIST
  ../src/define.cc
  ../src/math.cc
  ../src/object.cc
  ../src/objectlist.cc
)

SET (SRC_MANAGER_LIST
  ../src/manager/object.cc
  ../src/manager/objectsingle.cc
)


ADD_LIBRARY(plugin_structure SHARED
  ${SRC_LIST}
  ${SRC_MANAGER_LIST}
)

if(USE_32BITS)
  SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=i386 -mtune=i686")
  SET(CMAKE_CXX_FLAGS "-Wall -ggdb -pipe -march=i386 -mtune=i686")
else()
  SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=x86-64 -mtune=i686")
  SET(CMAKE_CXX_FLAGS "-Wall -ggdb -pipe -march=x86-64 -mtune=i686")
endif(USE_32BITS)

if (WIN32)
TARGET_LINK_LIBRARIES(plugin_structure ws2_32.lib pf_core.lib lua.lib iconv.lib)
else()
TARGET_LINK_LIBRARIES(plugin_structure pf_core lua iconv)
  if(USE_32BITS)
    SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=i386 -mtune=i686")
    SET(CMAKE_CXX_FLAGS "-Wall -ggdb -pipe -march=i386 -mtune=i686")
  else()
    SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=x86-64 -mtune=i686")
    SET(CMAKE_CXX_FLAGS "-Wall -ggdb -pipe -march=x86-64 -mtune=i686")
  endif(USE_32BITS)
endif(WIN32)
