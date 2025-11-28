project(LuneTests LANGUAGES CXX)

###########
# Options #
###########
enable_testing()

######################
# External libraries #
######################
find_package(Catch2 REQUIRED CONFIG)

#################################
# Set constants for the project #
#################################
file(GLOB_RECURSE TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

###############
# Build tests #
###############
message(STATUS "Found test sources: ${TEST_SOURCES}")
foreach(src ${TEST_SOURCES})
    get_filename_component(name "${src}" NAME_WE)

    add_executable(${name} ${src})
    target_link_libraries(${name}
            PRIVATE
            Lune
            Catch2::Catch2WithMain
    )

    add_test(NAME ${name} COMMAND ${name})
endforeach()
