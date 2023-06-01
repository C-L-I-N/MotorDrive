include_directories(${CMAKE_CURRENT_LIST_DIR}/Application)
include_directories(${CMAKE_CURRENT_LIST_DIR}/Middleware/Algorithm)

add_executable(
    state_machine_test
    state_machine_test.cpp
)
target_link_libraries(
    state_machine_test
    PUBLIC motor_drive_lib
    GTest::gtest_main
)

add_executable(
    trajectory_test
    trajectory_test.cpp
)
target_link_libraries(
    trajectory_test
    PUBLIC motor_drive_lib
    GTest::gtest_main
)

add_executable(
    controller_test
    controller_test.cpp
)
target_link_libraries(
    controller_test
    PUBLIC motor_drive_lib
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(state_machine_test)
gtest_discover_tests(trajectory_test)
gtest_discover_tests(controller_test)
