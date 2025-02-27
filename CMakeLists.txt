cmake_minimum_required(VERSION 3.11)
project(webServer)

set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -g -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated-declarations")
set(CMAKE_C_FLAGS "$ENV{CXXFLAGS} -rdynamic -O3 -fPIC -ggdb -std=c11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated-declarations")

find_package(Boost REQUIRED)

include_directories(apps/sylar/include)
link_directories(apps/sylar/lib)
find_library(YAMLCPP libyaml-cpp.a)


set(BASESRC 
  src/base/AsyncLogging.cc
  src/base/Condition.cc
  src/base/CountDownLatch.cc
  src/base/CurrentThread.cc
  src/base/Date.cc
  src/base/Exception.cc
  src/base/FileUtil.cc
  src/base/LogFile.cc
  src/base/Logging.cc
  src/base/LogStream.cc
  src/base/ProcessInfo.cc
  src/base/Timestamp.cc
  src/base/Thread.cc
  src/base/ThreadPool.cc
  src/base/TimeZone.cc
)


set(SYLAR src/sylar/config.cc src/sylar/log.cc src/sylar/thread.cc src/sylar/util.cc)

file(GLOB LIBSRC "src/*.cc")
#set(LIBSRC src/EventLoop.cc src/Channel.cc src/Poller.cc)

add_library(webServer SHARED ${LIBSRC} ${BASESRC} ${SYLAR})



add_executable(testWeb test/test.cc)
add_dependencies(testWeb webServer)
target_link_libraries(testWeb webServer yaml-cpp)

add_executable(testWeb2 test/test2.cc)
add_dependencies(testWeb2 webServer)
target_link_libraries(testWeb2 webServer yaml-cpp)

add_executable(testWeb7 test/test7.cc)
add_dependencies(testWeb7 webServer)
target_link_libraries(testWeb7 webServer yaml-cpp)

add_executable(testWeb8 test/test8.cc)
add_dependencies(testWeb8 webServer)
target_link_libraries(testWeb8 webServer yaml-cpp)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)