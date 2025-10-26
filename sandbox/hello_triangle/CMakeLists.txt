project(HelloTriangle LANGUAGES CXX)

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

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        Lune
)

target_include_directories(${PROJECT_NAME}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor
)

