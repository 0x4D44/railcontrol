# CMake generated Testfile for 
# Source directory: /home/user/railcontrol/tests
# Build directory: /home/user/railcontrol/build/coverage/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(railcore_basic_tests "/home/user/railcontrol/build/coverage/tests/railcore_basic_tests")
set_tests_properties(railcore_basic_tests PROPERTIES  WORKING_DIRECTORY "/home/user/railcontrol" _BACKTRACE_TRIPLES "/home/user/railcontrol/tests/CMakeLists.txt;72;add_test;/home/user/railcontrol/tests/CMakeLists.txt;0;")
add_test(railcore_tests "/home/user/railcontrol/build/coverage/tests/railcore_tests")
set_tests_properties(railcore_tests PROPERTIES  WORKING_DIRECTORY "/home/user/railcontrol" _BACKTRACE_TRIPLES "/home/user/railcontrol/tests/CMakeLists.txt;78;add_test;/home/user/railcontrol/tests/CMakeLists.txt;0;")
subdirs("../_deps/googletest-build")
