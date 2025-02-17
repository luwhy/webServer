cmake_minimum_required(VERSION 3.11)
project(webServer)

set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -g -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated-declarations")
set(CMAKE_C_FLAGS "$ENV{CXXFLAGS} -rdynamic -O3 -fPIC -ggdb -std=c11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated-declarations")

find_package(Boost REQUIRED)

include_directories(/app/sylar/include)
link_directories(/app/sylar/lib)
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

set(SYLAY src/sylar/config.cc src/sylar/log.cc src/sylar/thread.cc src/sylar/util.cc)


set(LIBSRC src/EventLoop.cc src/Channel.cc)

add_library(webServer SHARED ${LIBSRC} ${BASESRC})

add_executable(testWeb test/test.cc)

#这2者后面都是库
add_dependencies(testWeb webServer)
target_link_libraries(testWeb webServer -lyaml-cpp)


SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)