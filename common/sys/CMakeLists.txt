
file(GLOB sys_SRCS *.cc *.cpp *.h)
file(GLOB sys_PUBLIC_HEADERS *.h)

message(STATUS "sys_SRCS : " ${sys_SRCS})

SET(DEPENDENT_LIBRARIES glog)

set (MACOSX_RPATH ${LIBRARY_OUTPUT_PATH})

include_directories(${PROJECT_SOURCE_DIR})

add_library(sys_static STATIC ${sys_SRCS})
target_link_libraries(sys_static ${DEPENDENT_LIBRARIES})

if (UNIX)
    add_library(sys SHARED ${sys_SRCS})
    target_link_libraries(sys ${DEPENDENT_LIBRARIES})

    install (
      TARGETS sys sys_static
      EXPORT ${PACKAGE_NAME}
      RUNTIME DESTINATION bin
      LIBRARY DESTINATION lib
      ARCHIVE DESTINATION lib)

else (UNIX)
    install (
      TARGETS sys_static
      EXPORT ${PACKAGE_NAME}
      RUNTIME DESTINATION bin
      LIBRARY DESTINATION lib
      ARCHIVE DESTINATION lib)

endif (UNIX)

install (FILES ${sys_PUBLIC_HEADERS} DESTINATION "include/common/sys")
