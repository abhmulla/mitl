cmake_minimum_required(VERSION 3.25)

project(mitl VERSION 0.0.1 DESCRIPTION "Mulla In The Loop")

# Only build static libraries
option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)

# Disable MAVSDK testing
option(BUILD_TESTING "Build tests" OFF)

# Finds MAVSDK when installed system wide.
find_package(MAVSDK REQUIRED)

add_library(${PROJECT_NAME} STATIC
    src/mavlink_interface.cpp
    src/mode_manager.cpp
    src/navigator/navigator.cpp
    src/mode/takeoff.cpp
    src/vehicle/vehicle.cpp
    src/vehicle/controller.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}>

    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Set compile options
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)

# Define C++ standard:
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

# Link external libraries (MAVSDK) to the C++ library
target_link_libraries(${PROJECT_NAME} PUBLIC
    MAVSDK::mavsdk
)

# add examples
add_subdirectory(examples/)