project(Test)

# Define files to be compiled
set(TEST_FILES
    gazebo_test.cpp
    mavlink_interface_test.cpp
    mode_manager_test.cpp
)

enable_testing()

# Adding the tests target
add_executable(test ${TEST_FILES})

# Make catch2 available to this file
FetchContent_MakeAvailable(Catch2)

# Linking test with the Catch2 libraries
target_link_libraries(test PRIVATE mitl Catch2::Catch2WithMain)

# Add Catch2 extra modules to the CMAKE_MODULE_PATH
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

include(Catch)
catch_discover_tests(test)