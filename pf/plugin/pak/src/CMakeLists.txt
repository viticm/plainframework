SET(TF_CURRENT_BINARY_PATH "../dll")

IF(CMAKE_SYSTEM MATCHES Linux)

ELSE(CMAKE_SYSTEM MATCHES Linux)
ENDIF(CMAKE_SYSTEM MATCHES Linux)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})
LINK_DIRECTORIES(
  "./" #run dir
  "../../dependencies/lib" #linux
)

SET(EXECUTABLE_OUTPUT_PATH ${TF_CURRENT_BINARY_PATH_BIN})


INCLUDE_DIRECTORIES("../../../core/include",
                    "../include",
                    "../../../dependencies/iconv/include",
                    "../../../dependencies/lua/src",
                    "../../../dependencies/",
                    "/usr/local/unixODBC/include",
                    "../misc")

##update_begin




SET (SRC_LIST
	../src/Compress.cpp
	../src/kpmEncrypt.cpp
	../src/kpmLib.cpp
	../src/kpmLinux.cpp
	../src/kpmListFile.cpp
	../src/Util.cpp
)

SET (INCLUDE_LIST
	../include/kpmCompress.h
	../include/kpmEncrypt.h
	../include/kpmLib.h
	../include/kpmListFile.h
	../include/kpmPlatform.h
	../include/kpmTypes.h
	../include/Util.h
)

SET (MISC_BZIP2_LIST
	../misc/bzip2/blocksort.c
	../misc/bzip2/bzip2.c
	../misc/bzip2/bzlib.c
	../misc/bzip2/bzlib.h
	../misc/bzip2/bzlib_private.h
	../misc/bzip2/compress.c
	../misc/bzip2/crctable.c
	../misc/bzip2/decompress.c
	../misc/bzip2/huffman.c
	../misc/bzip2/randtable.c
)

SET (MISC_HUFFMAN_LIST
	../misc/huffman/huff.cpp
	../misc/huffman/huff.h
)

SET (MISC_PKZLIB_LIST
	../misc/pklib/crc32.c
	../misc/pklib/explode.c
	../misc/pklib/implode.c
	../misc/pklib/pklib.h
)

SET (MISC_WAVE_LIST
	../misc/wave/wave.cpp
	../misc/wave/wave.h
)

SET (MISC_LIST
	../misc/crc32.cpp
	../misc/crc32.h
	../misc/md5.cpp
	../misc/md5.h
)



if (WIN32)
	source_group(src FILES ${SRC_LIST})
	source_group(include FILES ${INCLUDE_LIST})
	source_group(misc\\bzip2 FILES ${MISC_BZIP2_LIST})
	source_group(misc\\huffman FILES ${MISC_HUFFMAN_LIST})
	source_group(misc\\pkzlib FILES ${MISC_PKZLIB_LIST})
	source_group(misc\\wave FILES ${MISC_WAVE_LIST})
	source_group(misc FILES ${MISC_LIST})
endif()


SET(LIBRARY_OUTPUT_PATH ${TF_CURRENT_BINARY_PATH})
ADD_LIBRARY(plugin_pak SHARED
	${SRC_LIST}
	${INCLUDE_LIST}
	${MISC_BZIP2_LIST}
	${MISC_HUFFMAN_LIST}
	${MISC_PKZLIB_LIST}
	${MISC_WAVE_LIST}
	${MISC_LIST}
)


##update_end

if (WIN32)
TARGET_LINK_LIBRARIES(plugin_pak ws2_32.lib odbc32.lib odbccp32.lib pf_lua.lib pf_iconv.lib)
else()
TARGET_LINK_LIBRARIES(plugin_pak pthread odbc pf_iconv pf_lua)
  if(USE_32BITS)
    SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=i386 -mtune=i686 -fPIC")
    SET(CMAKE_CXX_FLAGS "-Wall -ggdb -pipe -march=i386 -mtune=i686 -fPIC")
  else()
    SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=x86-64 -mtune=i686 -fPIC")
    SET(CMAKE_CXX_FLAGS "-Wall -ggdb -pipe -march=x86-64 -mtune=i686 -fPIC")
  endif(USE_32BITS)
endif(WIN32)
