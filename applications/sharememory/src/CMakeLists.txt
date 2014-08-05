SET(TF_CURRENT_BINARY_PATH_BIN "../../bin")
ADD_DEFINITIONS(-D_SHAREMEMORY)

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
)

SET(EXECUTABLE_OUTPUT_PATH ${TF_CURRENT_BINARY_PATH_BIN})


INCLUDE_DIRECTORIES("../../../pf/core/include",
                    "../include",
                    "../../common/include",
                    "../../../pf/dependencies/iconv/include",
                    "../../../pf/dependencies/lua/src",
                    "/usr/local/unixODBC/include")

##update_begin




SET (SOURCEFILES_ARCHIVE_DATA_LIST
	../src/archive/data/globaldata.cc
	../src/archive/data/interface.cc
)

SET (SOURCEFILES_ARCHIVE_NODE_LOGICS_LIST
	../src/archive/node/logics/globaldata.cc
)

SET (SOURCEFILES_ARCHIVE_NODE_LIST
	../src/archive/node/logicmanager.cc
)

SET (SOURCEFILES_ARCHIVE_LIST
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

SET (HEADERFILES_ARCHIVE_NODE_LIST
	../include/archive/node/config.h
	../include/archive/node/logic.h
	../include/archive/node/logicmanager.h
)

SET (HEADERFILES_ARCHIVE_DATA_LIST
	../include/archive/data/config.h
	../include/archive/data/globaldata.h
	../include/archive/data/interface.h
)

SET (HEADERFILES_ARCHIVE_LIST
	../include/archive/config.h
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

SET (COMMON_SRC_DB_LIST
	../../common/src/db/define.cc
)

SET (COMMON_SRC_SHAREMEMORY_LIST
	../../common/src/sharememory/define.cc
)

SET (COMMON_SRC_LIST
	../../common/src/setting.cc
)

SET (COMMON_INCLUDE_DEFINE_LIST
	../../common/include/common/define/config.h
	../../common/include/common/define/file.h
	../../common/include/common/define/log.h
	../../common/include/common/define/macros.h
)

SET (COMMON_INCLUDE_DB_LIST
	../../common/include/common/db/config.h
	../../common/include/common/db/define.h
)

SET (COMMON_INCLUDE_SHAREMEMORY_LIST
	../../common/include/common/sharememory/config.h
	../../common/include/common/sharememory/define.h
)

SET (COMMON_INCLUDE_LIST
	../../common/include/common/config.h
	../../common/include/common/setting.h
)

SET (COMMON_LIST
)



if (WIN32)
	source_group(SourceFiles\\archive\\data FILES ${SOURCEFILES_ARCHIVE_DATA_LIST})
	source_group(SourceFiles\\archive\\node\\logics FILES ${SOURCEFILES_ARCHIVE_NODE_LOGICS_LIST})
	source_group(SourceFiles\\archive\\node FILES ${SOURCEFILES_ARCHIVE_NODE_LIST})
	source_group(SourceFiles\\archive FILES ${SOURCEFILES_ARCHIVE_LIST})
	source_group(SourceFiles\\engine FILES ${SOURCEFILES_ENGINE_LIST})
	source_group(SourceFiles FILES ${SOURCEFILES_LIST})
	source_group(HeaderFiles\\application FILES ${HEADERFILES_APPLICATION_LIST})
	source_group(HeaderFiles\\archive\\node FILES ${HEADERFILES_ARCHIVE_NODE_LIST})
	source_group(HeaderFiles\\archive\\data FILES ${HEADERFILES_ARCHIVE_DATA_LIST})
	source_group(HeaderFiles\\archive FILES ${HEADERFILES_ARCHIVE_LIST})
	source_group(HeaderFiles\\engine FILES ${HEADERFILES_ENGINE_LIST})
	source_group(HeaderFiles FILES ${HEADERFILES_LIST})
	source_group(ResourceFiles FILES ${RESOURCEFILES_LIST})
	source_group(common\\src\\db FILES ${COMMON_SRC_DB_LIST})
	source_group(common\\src\\sharememory FILES ${COMMON_SRC_SHAREMEMORY_LIST})
	source_group(common\\src FILES ${COMMON_SRC_LIST})
	source_group(common\\include\\define FILES ${COMMON_INCLUDE_DEFINE_LIST})
	source_group(common\\include\\db FILES ${COMMON_INCLUDE_DB_LIST})
	source_group(common\\include\\sharememory FILES ${COMMON_INCLUDE_SHAREMEMORY_LIST})
	source_group(common\\include FILES ${COMMON_INCLUDE_LIST})
	source_group(common FILES ${COMMON_LIST})
endif()


ADD_EXECUTABLE(sharememory
	${SOURCEFILES_ARCHIVE_DATA_LIST}
	${SOURCEFILES_ARCHIVE_NODE_LOGICS_LIST}
	${SOURCEFILES_ARCHIVE_NODE_LIST}
	${SOURCEFILES_ARCHIVE_LIST}
	${SOURCEFILES_ENGINE_LIST}
	${SOURCEFILES_LIST}
	${HEADERFILES_APPLICATION_LIST}
	${HEADERFILES_ARCHIVE_NODE_LIST}
	${HEADERFILES_ARCHIVE_DATA_LIST}
	${HEADERFILES_ARCHIVE_LIST}
	${HEADERFILES_ENGINE_LIST}
	${HEADERFILES_LIST}
	${RESOURCEFILES_LIST}
	${COMMON_SRC_DB_LIST}
	${COMMON_SRC_SHAREMEMORY_LIST}
	${COMMON_SRC_LIST}
	${COMMON_INCLUDE_DEFINE_LIST}
	${COMMON_INCLUDE_DB_LIST}
	${COMMON_INCLUDE_SHAREMEMORY_LIST}
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
TARGET_LINK_LIBRARIES(sharememory ws2_32.lib odbc32.lib odbccp32.lib pf_core.lib)
else()
TARGET_LINK_LIBRARIES(sharememory pthread odbc pf_core)
  if(USE_32BITS)
    SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=i386 -mtune=i686")
    SET(CMAKE_CXX_FLAGS "-Wall -ggdb -pipe -march=i386 -mtune=i686")
  else()
    SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=x86-64 -mtune=i686")
    SET(CMAKE_CXX_FLAGS "-Wall -ggdb -pipe -march=x86-64 -mtune=i686")
  endif(USE_32BITS)
endif(WIN32)
