cmake_minimum_required(VERSION 3.5)

project(CommonInclude C)

add_library(${PROJECT_NAME}
  src/CommonInclude.c
  )

target_include_directories(${PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  )
