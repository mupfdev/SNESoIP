cmake_minimum_required(VERSION 2.6.0)

project(server C)

add_executable(server src/main.c src/inih/ini.c)

configure_file(config.ini config.ini COPYONLY)

target_compile_options(server PUBLIC -O2 -Wall -Wextra -Werror)
