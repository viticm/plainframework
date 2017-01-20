# This file generate by plain framework admin tool, date: 2017-01-20 00:28:32
SET(LIBRARY_OUTPUT_PATH ./../../../dll/)

ADD_DEFINITIONS(-DPF_PLUGIN_CORE)
ADD_DEFINITIONS(-DPF_BUILD_AS_DLL)
ADD_DEFINITIONS(-DLUA_COMPAT_MODULE)
SET(USE_32BITS 0)

IF(CMAKE_SYSTEM MATCHES Linux)
  ADD_DEFINITIONS(-D_REENTRANT)
  ADD_DEFINITIONS(-DDONT_TD_VOID)
ELSE(CMAKE_SYSTEM MATCHES Linux)
ENDIF(CMAKE_SYSTEM MATCHES Linux)

LINK_DIRECTORIES(
  "./"
  "./../../../dependencies/dll/"
  "./../../../core/dll/"
)

INCLUDE_DIRECTORIES(
  "../include/"
  "../../../../pf/core/include/"
  "../../../../pf/dependencies/lua/5.3.0/include/"
)


SET (INCLUDE_LIST
  ../include/cache.h
  ../include/config.h
  ../include/export.h
  ../include/file.h
  ../include/logger.h
)

SET (SRC_LIST
  ../src/cache.cc
  ../src/export.cc
  ../src/file.cc
  ../src/logger.cc
)


ADD_LIBRARY(pf_plugin_lua SHARED
  ${SRC_LIST}
)

if (WIN32)
TARGET_LINK_LIBRARIES(pf_plugin_lua ws2_32.lib pf_core.lib lua.lib)
else()
TARGET_LINK_LIBRARIES(pf_plugin_lua pf_core lua)
  if(USE_32BITS)
    SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=i386 -mtune=i686")
    SET(CMAKE_CXX_FLAGS "-std=c++11 -Wall -ggdb -pipe -march=i386 -mtune=i686")
  else()
    SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=x86-64 -mtune=i686")
    SET(CMAKE_CXX_FLAGS "-std=c++11 -Wall -ggdb -pipe -march=x86-64 -mtune=i686")
  endif(USE_32BITS)
endif(WIN32)
