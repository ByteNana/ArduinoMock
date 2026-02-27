cmake_minimum_required(VERSION 3.15)

include(FetchContent)

option(ENABLE_GTEST_TESTS "Build GoogleTest/GoogleMock tests" ON)

if(ENABLE_GTEST_TESTS)

  # --- GoogleTest/GoogleMock ---
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP true
  )
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)

  function(add_gtest name)
    add_executable(${name} ${ARGN})
    target_link_libraries(${name} PRIVATE ArduinoNativeMocks GTest::gmock_main)
    target_include_directories(${name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)
    add_test(NAME ${name} COMMAND ${name})
  endfunction()

  add_gtest(queue_gtest queue_gtest.cpp)
  add_gtest(semaphore_gtest semaphore_gtest.cpp)
  add_gtest(task_gtest task_gtest.cpp)
  add_gtest(stream_gmock stream_gmock_test.cpp)
  add_gtest(config_assert_gtest config_assert_gtest.cpp)
  add_gtest(md5builder_gtest md5builder_gtest.cpp)
  add_gtest(times_gtest times_gtest.cpp)
endif()
