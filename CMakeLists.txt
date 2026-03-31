cmake_minimum_required(VERSION 3.15)
project(IdfMock C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(idf_mock STATIC
    src/task.c
    src/queue.c
    src/semphr.c
    src/timers.c
    src/esp_log.c
)

target_include_directories(idf_mock PUBLIC include)
target_link_libraries(idf_mock PUBLIC pthread)
target_compile_options(idf_mock PRIVATE -Wall -Wextra -Wpedantic)

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(test)
endif()
