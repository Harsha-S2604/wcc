cmake_minimum_required(VERSION 3.31.6)
project(wcc)
add_executable(${PROJECT_NAME} main.c)
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
