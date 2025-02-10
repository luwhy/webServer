cmake_minimum_required(VERSION 3.11)
project(webServer)

set(LIBSRC src/EventLoop.cc src/CurrentThread.cc)

add_library(webServer SHARED ${LIBSRC})

add_executable(testWeb test/test.cc)

#这2者后面都是库

add_dependencies(testWeb webServer)
target_link_libraries(testWeb webServer)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH &{PROJECT_SOURCE_DIR}/lib)