cmake_minimum_required(VERSION 3.5)

project(server C)

find_package(Threads)
add_executable(${PROJECT_NAME}
    src/server.c
    src/inih/ini.c
    )

add_subdirectory(../Tools/CommonInclude CommonInclude)

target_link_libraries(${PROJECT_NAME}
  ${CMAKE_THREAD_LIBS_INIT}
  CommonInclude
  )

configure_file(config.ini config.ini COPYONLY)

target_compile_options(${PROJECT_NAME} PUBLIC -O2 -pedantic-errors -Wall -Wextra -Werror)
