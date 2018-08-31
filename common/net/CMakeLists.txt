
file(GLOB net_SRCS *.cc *.h)
file(GLOB net_PUBLIC_HEADERS *.h)

if (UNIX)
    SET(DEPENDENT_LIBRARIES event glog pthread evpp_concurrentqueue)
else (UNIX)
    SET(DEPENDENT_LIBRARIES event glog)
endif (UNIX)

if (NOT EVPP_VCPKG_BUILD)
    add_subdirectory (test)
endif ()

message(STATUS "net_SRCS : " ${net_SRCS})

set (MACOSX_RPATH ${LIBRARY_OUTPUT_PATH})

include_directories(${PROJECT_SOURCE_DIR})

add_library(net_static STATIC ${net_SRCS})
target_link_libraries(net_static ${DEPENDENT_LIBRARIES})

if (UNIX)
    add_library(net SHARED ${net_SRCS})
    target_link_libraries(net ${DEPENDENT_LIBRARIES})

    install (
      TARGETS net net_static
      EXPORT ${PACKAGE_NAME}
      RUNTIME DESTINATION bin
      LIBRARY DESTINATION lib
      ARCHIVE DESTINATION lib)

else (UNIX)
    install (
      TARGETS net_static
      EXPORT ${PACKAGE_NAME}
      RUNTIME DESTINATION bin
      LIBRARY DESTINATION lib
      ARCHIVE DESTINATION lib)

endif (UNIX)

install (FILES ${net_PUBLIC_HEADERS} DESTINATION "include/common/net")
