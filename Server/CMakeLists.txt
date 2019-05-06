cmake_minimum_required(VERSION 2.6.0)

project(server C)

find_package(Threads)
add_executable(${PROJECT_NAME}
    src/server.c
    src/inih/ini.c
    src/CommonInclude/CommonInclude.c
    )

target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/src/CommonInclude)

configure_file(config.ini config.ini COPYONLY)

target_compile_options(${PROJECT_NAME} PUBLIC -O2 -pedantic-errors -Wall -Wextra -Werror)
