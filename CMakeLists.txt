cmake_minimum_required(VERSION 3.30.1)
project(CodeCPas C)

set(CMAKE_C_STANDARD 99)

add_executable(server server.c)
add_executable(client client.c)

find_package(SQLite3 REQUIRED)
target_link_libraries(server SQLite::SQLite3)
target_link_libraries(server pthread)

set(CHECK_INCLUDE_DIRS "/opt/homebrew/opt/check/include")
set(CHECK_LIBRARIES "/opt/homebrew/opt/check/lib/libcheck.a")

include_directories(${CHECK_INCLUDE_DIRS})

enable_testing()

add_executable(test_client_server test_client_server.c)

target_link_libraries(test_client_server ${CHECK_LIBRARIES} pthread SQLite::SQLite3)

add_test(NAME ClientServerTests COMMAND test_client_server)
