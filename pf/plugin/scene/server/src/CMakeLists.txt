# This file generate by plain framework admin tool, date: 2015-05-09 13:21:46
SET(LIBRARY_OUTPUT_PATH ./../../../../dll/)

ADD_DEFINITIONS(-DPF_PLUGIN_CORE)
SET(USE_32BITS 0)

IF(CMAKE_SYSTEM MATCHES Linux)
  ADD_DEFINITIONS(-D_REENTRANT)
  ADD_DEFINITIONS(-DDONT_TD_VOID)
  ADD_DEFINITIONS(-D_PF_NET_EPOLL)
ELSE(CMAKE_SYSTEM MATCHES Linux)
ENDIF(CMAKE_SYSTEM MATCHES Linux)

LINK_DIRECTORIES(
  "./../../../../dependencies/dll/"
  "./../../../../dll/"
  "./../../../../core/dll/"
)

INCLUDE_DIRECTORIES(
  "../include/"
  "../../../../dependencies/lua/include/"
  "../../../../dependencies/iconv/include/"
  "../../../../core/include/"
  "../../../../plugin/structure/include/"
)


SET (INCLUDE_SCENE_LIST
  ../include/scene/chat_pipeline.h
  ../include/scene/config.h
  ../include/scene/define.h
  ../include/scene/grid.h
  ../include/scene/map.h
  ../include/scene/pathfinder.h
  ../include/scene/scaner.h
  ../include/scene/scene.h
  ../include/scene/thread.h
  ../include/scene/timer.h
)

SET (INCLUDE_SCENE_MANAGER_LIST
  ../include/scene/manager/config.h
  ../include/scene/manager/dropposition.h
  ../include/scene/manager/fuben.h
  ../include/scene/manager/net.h
  ../include/scene/manager/pkregion.h
  ../include/scene/manager/recycleconnection.h
  ../include/scene/manager/region.h
  ../include/scene/manager/scene.h
  ../include/scene/manager/thread.h
  ../include/scene/manager/timerdoing.h
  ../include/scene/manager/variable.h
)

SET (SRC_LIST
  ../src/chat_pipeline.cc
  ../src/define.cc
  ../src/grid.cc
  ../src/map.cc
  ../src/pathfinder.cc
  ../src/scaner.cc
  ../src/scene.cc
  ../src/thread.cc
  ../src/timer.cc
)

SET (SRC_MANAGER_LIST
  ../src/manager/dropposition.cc
  ../src/manager/fuben.cc
  ../src/manager/net.cc
  ../src/manager/pkregion.cc
  ../src/manager/recycleconnection.cc
  ../src/manager/region.cc
  ../src/manager/scene.cc
  ../src/manager/thread.cc
  ../src/manager/timerdoing.cc
  ../src/manager/variable.cc
)


ADD_LIBRARY(plugin_scene SHARED
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
TARGET_LINK_LIBRARIES(plugin_scene ws2_32.lib odbc32.lib odbccp32.lib pf_core.lib lua.lib iconv.lib plugin_structure.lib)
else()
TARGET_LINK_LIBRARIES(plugin_scene odbc pf_core lua iconv plugin_structure)
  if(USE_32BITS)
    SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=i386 -mtune=i686")
    SET(CMAKE_CXX_FLAGS "-Wall -ggdb -pipe -march=i386 -mtune=i686")
  else()
    SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=x86-64 -mtune=i686")
    SET(CMAKE_CXX_FLAGS "-Wall -ggdb -pipe -march=x86-64 -mtune=i686")
  endif(USE_32BITS)
endif(WIN32)
