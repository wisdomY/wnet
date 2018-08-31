file(GLOB echo_server_SRCS echoserver.cc)
file(GLOB echo_client_SRCS echoclient.cc)

if (MSVC)
link_directories(${LIBRARY_OUTPUT_PATH}/${CMAKE_BUILD_TYPE}/)
endif (MSVC)

add_executable(echo_server ${echo_server_SRCS})
target_link_libraries(echo_server net_static ${DEPENDENT_LIBRARIES})

add_executable(echo_client ${echo_client_SRCS})
target_link_libraries(echo_client net_static ${DEPENDENT_LIBRARIES})
