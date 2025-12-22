cmake_minimum_required(VERSION 3.25)

project(mitlex)

# Define files to be compiled
set(MITL_FILES
    main.cpp
    Takeoff.cpp
    gazebo.cpp
)

# Build and link all executables:

foreach( utilfile ${MITL_FILES} )

    # Determine a target name

    string( REPLACE ".cpp" "" utilname ${utilfile} )

    # Add the executible for this test:

    add_executable( ${utilname} ${utilfile} )

    # Link mitl

    target_link_libraries( ${utilname} mitl )

endforeach( utilfile ${MITL_FILES} )