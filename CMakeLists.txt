cmake_minimum_required(VERSION 3.15)
project(ArduinoNativeMocks VERSION 0.1.1 LANGUAGES C CXX)

set(PROJECT_VERSION_SUFIX "-rc.1")
set(CMAKE_PROJECT_VERSION "${PROJECT_VERSION}${PROJECT_VERSION_SUFIX}")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 11)
set(LOG_LEVEL "3" CACHE STRING "Log level (0-5, where 5 is most verbose)")

set(CMAKE_BUILD_TYPE Debug)

# Paths
set(LIB_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)


file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
  ${LIB_SRC_DIR}/*.cpp
  ${LIB_SRC_DIR}/*.h
  ${LIB_SRC_DIR}/*.c
)

add_library(ArduinoNativeMocks STATIC ${SRC_FILES})

target_include_directories(ArduinoNativeMocks
  PUBLIC ${LIB_SRC_DIR}
)

find_package(Threads REQUIRED)
target_link_libraries(ArduinoNativeMocks PUBLIC Threads::Threads)

# For MD5Builder: use native crypto on non-Apple platforms
if(NOT APPLE)
  find_package(OpenSSL REQUIRED)
  target_compile_definitions(ArduinoNativeMocks PUBLIC HAVE_OPENSSL_MD5=1)
  target_link_libraries(ArduinoNativeMocks PUBLIC OpenSSL::Crypto)
endif()

enable_testing()
add_subdirectory(examples)
add_subdirectory(test)
