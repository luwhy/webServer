cmake_minimum_required(VERSION 3.11)
project(webServer)

set(CXX_FLAGS -g -std=c++11 -rdynamic)

find_package(Boost REQUIRED)
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
set(LIBSRC src/EventLoop.cc src/Thread.cc)

add_library(webServer SHARED ${LIBSRC} ${BASESRC})
target_link_libraries(webServer pthread Boost)

add_executable(testWeb test/test.cc)

#这2者后面都是库
add_dependencies(testWeb webServer)
target_link_libraries(testWeb webServer)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH &{PROJECT_SOURCE_DIR}/lib)