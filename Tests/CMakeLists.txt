include_directories(${CMAKE_CURRENT_LIST_DIR}/Application)

add_executable(
    state_machine_test
    state_machine_test.cpp
)
target_link_libraries(
    state_machine_test
    PUBLIC motor_drive_lib
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(state_machine_test)
