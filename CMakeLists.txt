cmake_minimum_required (VERSION 2.8)

project (qy-lightControlDag C CXX)

if (UNIX)
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Release")
    endif()

endif (UNIX)


set (EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set (LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

#set (MACOSX_RPATH ${LIBRARY_OUTPUT_PATH})
set (CMAKE_MACOSX_RPATH 0)

add_subdirectory (libs/evpp)
add_subdirectory (libs/websocketpp)
if (UNIX)
    execute_process ( COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/checkout-protobuf.sh )
else (UNIX)
    #message( FATAL_ERROR "You must checkout protobuf v3.0.0 tag on Windows, CMake will exit." )
	execute_process ( COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/checkout-protobuf.bat )
endif (UNIX)
set(protobuf_BUILD_TESTS OFF CACHE BOOL "Build tests")
add_subdirectory (libs/protobuf/cmake)
add_subdirectory (common)
add_subdirectory (gateway)
add_subdirectory (app)
include (CPack)
include (CTest)
