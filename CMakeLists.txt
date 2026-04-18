cmake_minimum_required(VERSION 3.23)
project(paperchat)

find_package(libsocket REQUIRED)

file(GLOB_RECURSE CLIENT_SOURCES CONFIGURE_DEPENDS "src/util.*" "src/client/*.h" "src/client/*.c")
add_executable(client ${CLIENT_SOURCES})
target_link_libraries(client PRIVATE libsocket::socket getopt)

file(GLOB_RECURSE SERVER_SOURCES CONFIGURE_DEPENDS "src/util.*" "src/server/*.h" "src/server/*.c")
add_executable(server ${SERVER_SOURCES})
target_link_libraries(server PRIVATE libsocket::socket getopt)