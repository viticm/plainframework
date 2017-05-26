# This file generate by plain framework admin tool, date: 2017-05-21 12:54:22
SET(LIBRARY_OUTPUT_PATH ./../../dll/)

ADD_DEFINITIONS(-DICONV_CORE)
ADD_DEFINITIONS(-DCHARSET_CORE)
SET(USE_32BITS 0)

IF(CMAKE_SYSTEM MATCHES Linux)
  ADD_DEFINITIONS(-D_REENTRANT)
  ADD_DEFINITIONS(-DDONT_TD_VOID)
  ADD_DEFINITIONS(-DLUA_USE_LINUX)
ELSE(CMAKE_SYSTEM MATCHES Linux)
ENDIF(CMAKE_SYSTEM MATCHES Linux)

LINK_DIRECTORIES(
  "./"
)

INCLUDE_DIRECTORIES(
  "../include/"
)


SET (INCLUDE_LIST
  ../include/iconv.h
)

SET (SRC_LIST
  ../src/iconv.c
  ../src/localcharset.c
  ../src/relocatable.c
)


ADD_LIBRARY(iconv SHARED
  ${SRC_LIST}
)

if (WIN32)
TARGET_LINK_LIBRARIES(iconv ws2_32.lib )
else()
TARGET_LINK_LIBRARIES(iconv )
  if(USE_32BITS)
    SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=i386 -mtune=i686")
    SET(CMAKE_CXX_FLAGS "-std=c++11 -Wall -ggdb -pipe -march=i386 -mtune=i686")
  else()
    SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=x86-64 -mtune=i686")
    SET(CMAKE_CXX_FLAGS "-std=c++11 -Wall -ggdb -pipe -march=x86-64 -mtune=i686")
  endif(USE_32BITS)
endif(WIN32)
