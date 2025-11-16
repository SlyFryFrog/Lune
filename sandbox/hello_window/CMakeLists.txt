project(HelloWindow LANGUAGES CXX)

include (${CMAKE_SOURCE_DIR}/cmake/MetalSetup.cmake)

#################################
# Set constants for the project #
#################################
file(GLOB_RECURSE SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

file(GLOB_RECURSE MODULES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cppm
)

######################
# Target: executable #
######################
add_executable(${PROJECT_NAME} ${SOURCES})

target_sources(${PROJECT_NAME}
        PUBLIC
        FILE_SET allModules
        TYPE CXX_MODULES
        FILES ${MODULES}
)

find_package(glfw3 CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        Lune
        glfw
)

target_include_directories(${PROJECT_NAME}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor
)

setup_metal_for_target(${PROJECT_NAME})
