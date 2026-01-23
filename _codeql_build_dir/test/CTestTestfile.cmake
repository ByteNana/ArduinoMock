# CMake generated Testfile for 
# Source directory: /home/runner/work/ArduinoMock/ArduinoMock/test
# Build directory: /home/runner/work/ArduinoMock/ArduinoMock/_codeql_build_dir/test
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(queue_gtest "/home/runner/work/ArduinoMock/ArduinoMock/_codeql_build_dir/test/queue_gtest")
set_tests_properties(queue_gtest PROPERTIES  _BACKTRACE_TRIPLES "/home/runner/work/ArduinoMock/ArduinoMock/test/CMakeLists.txt;23;add_test;/home/runner/work/ArduinoMock/ArduinoMock/test/CMakeLists.txt;26;add_gtest;/home/runner/work/ArduinoMock/ArduinoMock/test/CMakeLists.txt;0;")
add_test(semaphore_gtest "/home/runner/work/ArduinoMock/ArduinoMock/_codeql_build_dir/test/semaphore_gtest")
set_tests_properties(semaphore_gtest PROPERTIES  _BACKTRACE_TRIPLES "/home/runner/work/ArduinoMock/ArduinoMock/test/CMakeLists.txt;23;add_test;/home/runner/work/ArduinoMock/ArduinoMock/test/CMakeLists.txt;27;add_gtest;/home/runner/work/ArduinoMock/ArduinoMock/test/CMakeLists.txt;0;")
add_test(task_gtest "/home/runner/work/ArduinoMock/ArduinoMock/_codeql_build_dir/test/task_gtest")
set_tests_properties(task_gtest PROPERTIES  _BACKTRACE_TRIPLES "/home/runner/work/ArduinoMock/ArduinoMock/test/CMakeLists.txt;23;add_test;/home/runner/work/ArduinoMock/ArduinoMock/test/CMakeLists.txt;28;add_gtest;/home/runner/work/ArduinoMock/ArduinoMock/test/CMakeLists.txt;0;")
add_test(stream_gmock "/home/runner/work/ArduinoMock/ArduinoMock/_codeql_build_dir/test/stream_gmock")
set_tests_properties(stream_gmock PROPERTIES  _BACKTRACE_TRIPLES "/home/runner/work/ArduinoMock/ArduinoMock/test/CMakeLists.txt;23;add_test;/home/runner/work/ArduinoMock/ArduinoMock/test/CMakeLists.txt;29;add_gtest;/home/runner/work/ArduinoMock/ArduinoMock/test/CMakeLists.txt;0;")
add_test(times_gtest "/home/runner/work/ArduinoMock/ArduinoMock/_codeql_build_dir/test/times_gtest")
set_tests_properties(times_gtest PROPERTIES  _BACKTRACE_TRIPLES "/home/runner/work/ArduinoMock/ArduinoMock/test/CMakeLists.txt;23;add_test;/home/runner/work/ArduinoMock/ArduinoMock/test/CMakeLists.txt;30;add_gtest;/home/runner/work/ArduinoMock/ArduinoMock/test/CMakeLists.txt;0;")
subdirs("../_deps/googletest-build")
