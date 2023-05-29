cmake_minimum_required(VERSION 3.14)
project(MotorrDrive)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

AUX_SOURCE_DIRECTORY(Application APPLICATION_DIR_LIST)
add_library(motor_drive_lib ${APPLICATION_DIR_LIST}
                            ${CMAKE_CURRENT_LIST_DIR}/Common/DriveProtocol/src/crc8.c
                            ${CMAKE_CURRENT_LIST_DIR}/Common/DriveProtocol/src/com_slave.c)

target_include_directories(motor_drive_lib PUBLIC ${CMAKE_CURRENT_LIST_DIR}/Application
                                                  ${CMAKE_CURRENT_LIST_DIR}/Common/DriveProtocol/inc)

add_subdirectory(Tests)
