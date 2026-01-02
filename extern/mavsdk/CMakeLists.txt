cmake_minimum_required(VERSION 3.10.2)

include(FetchContent)

set(PACKAGE_NAME MAVSDK)

FetchContent_Declare(
    ${PACKAGE_NAME}
    GIT_REPOSITORY https://github.com/mavlink/MAVSDK.git
    GIT_TAG v3.2.0
    SYSTEM
)

FetchContent_MakeAvailable(${PACKAGE_NAME})