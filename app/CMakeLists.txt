cmake_minimum_required (VERSION 2.8)

if (UNIX)
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Release")
    endif()

    set (CXX_FLAGS
         -g
         -fPIC
         -Wall
         -Wno-unused-parameter
         -Wno-unused-function
         -Wunused-variable
         -Wunused-value
         -Wshadow
         -Wcast-qual
         -Wcast-align
         -Wwrite-strings
         -Wsign-compare
         -Winvalid-pch
         -fms-extensions
         -Wfloat-equal
         -Wextra
         -std=c++11
    )
    include_directories (${PROJECT_SOURCE_DIR}
                         ${PROJECT_SOURCE_DIR}/libs/evpp
                         ${PROJECT_SOURCE_DIR}/libs/boost/boost_1_67_0/boost
                         ${PROJECT_SOURCE_DIR}/libs/protobuf
                         ${PROJECT_SOURCE_DIR}/libs/websocketpp
                         ${PROJECT_SOURCE_DIR}/common
                         ${PROJECT_SOURCE_DIR}/gateway
                         ${PROJECT_SOURCE_DIR}/gateway/serialport
                         ${PROJECT_SOURCE_DIR}/gateway/websocketserver)
else (UNIX)
set (CXX_FLAGS -DWIN32 -D_WIN32 -DTIXML_USE_STL -EHsc -D_X86_)
    set(VcpkgRoot ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET})
    include_directories(${VcpkgRoot}/include
                        ${PROJECT_SOURCE_DIR}
                        ${PROJECT_SOURCE_DIR}/libs/evpp
                        ${PROJECT_SOURCE_DIR}/libs/boost/boost_1_67_0/boost
                        ${PROJECT_SOURCE_DIR}/libs/protobuf
                        ${PROJECT_SOURCE_DIR}/libs/websocketpp
                        ${PROJECT_SOURCE_DIR}/common
                        ${PROJECT_SOURCE_DIR}/gateway
                        ${PROJECT_SOURCE_DIR}/gateway/serialport
                        ${PROJECT_SOURCE_DIR}/gateway/websocketserver)
	
    # On windows, Debug mode
    link_directories(${VcpkgRoot}/debug/lib ${VcpkgRoot}/debug/lib/manual-link)

    # On Windows, Release mode
    # link_directories(${VcpkgRoot}/lib ${VcpkgRoot}/lib/manual-link)
endif (UNIX)

string (REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

if (UNIX)
    SET(CMAKE_CXX_FLAGS_DEBUG   "-O0 -g -ggdb -D_DEBUG -DGOOGLE_STRIP_LOG=0")
    SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -g -ggdb -DNDEBUG -DGOOGLE_STRIP_LOG=1")
endif (UNIX)

#if (NOT EXECUTABLE_OUTPUT_PATH)
#    set (EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
#endif ()
#if (NOT LIBRARY_OUTPUT_PATH)
#   set (LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
#endif ()


message (STATUS "This is BINARY dir " ${PROJECT_BINARY_DIR})
message (STATUS "This is SOURCE dir " ${PROJECT_SOURCE_DIR})
message (STATUS "CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS})
message (STATUS "EXECUTABLE_OUTPUT_PATH " ${EXECUTABLE_OUTPUT_PATH})
message (STATUS "LIBRARY_OUTPUT_PATH " ${LIBRARY_OUTPUT_PATH})

#file(GLOB gateway_SRCS *.cc *.cpp *.h)
#file(GLOB gateway_PUBLIC_HEADERS *.h)
file(GLOB app_SRCS *.cpp *.cc *.h)
set(app_SRCS ${app_SRCS})
message(STATUS "app_SRCS : " ${app_SRCS})

SET(DEPENDENT_LIBRARIES glog evpp_static libprotobuf net_static sys_static gateway_static )

set (MACOSX_RPATH ${LIBRARY_OUTPUT_PATH})

#add_library(gateway_static STATIC ${gateway_SRCS})
add_executable(app ${app_SRCS} )
target_link_libraries(app ${DEPENDENT_LIBRARIES})

install (
  TARGETS app
  EXPORT ${PACKAGE_NAME}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

include (CPack)
include (CTest)
