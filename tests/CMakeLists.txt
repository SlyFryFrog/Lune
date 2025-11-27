cmake_minimum_required(VERSION 3.20)
project(LuneTests LANGUAGES CXX)

###########
# Options #
###########
enable_testing()

######################
# External libraries #
######################
find_package(Catch2 3 REQUIRED CONFIG)

#################################
# Set constants for the project #
#################################
file(GLOB_RECURSE TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

###################
# Target: library #
###################
add_library(LuneTestLib INTERFACE)
target_include_directories(LuneTestLib INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_compile_features(LuneTestLib INTERFACE cxx_std_20)


###############
# Build tests #
###############
message(STATUS "Found test sources: ${TEST_SOURCES}")
foreach(src ${TEST_SOURCES})
    get_filename_component(name "${src}" NAME_WE)

    add_executable(${name} ${src})
    target_link_libraries(${name}
            PRIVATE
            LuneTestLib
            Lune
            Catch2::Catch2WithMain
    )

    add_test(NAME ${name} COMMAND ${name})
endforeach()
