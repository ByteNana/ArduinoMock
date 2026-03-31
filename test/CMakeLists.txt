cmake_minimum_required(VERSION 3.15)

include(FetchContent)

FetchContent_Declare(
    unity
    URL https://github.com/ThrowTheSwitch/Unity/archive/refs/tags/v2.6.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP true
)
FetchContent_MakeAvailable(unity)

function(add_unity_test name src)
    add_executable(${name} ${src})
    target_link_libraries(${name} PRIVATE idf_mock unity pthread)
    target_include_directories(${name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
    add_test(NAME ${name} COMMAND ${name})
endfunction()

add_unity_test(test_task    test_task.c)
add_unity_test(test_queue   test_queue.c)
add_unity_test(test_semphr  test_semphr.c)
add_unity_test(test_timers  test_timers.c)
