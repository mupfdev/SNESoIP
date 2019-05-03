cmake_minimum_required(VERSION 2.6.0)

project(server C)

find_package(Threads)
add_executable(server src/server.c src/inih/ini.c)
target_link_libraries(server ${CMAKE_THREAD_LIBS_INIT} )

configure_file(config.ini config.ini COPYONLY)

target_compile_options(server PUBLIC -O2 -pedantic-errors -Wall -Wextra -Werror)
