project(MetalSandbox LANGUAGES CXX)

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

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    foreach(dir shaders)
        add_custom_command(
                TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E remove_directory $<TARGET_FILE_DIR:${PROJECT_NAME}>/${dir}
                COMMAND ${CMAKE_COMMAND} -E create_symlink
                ${CMAKE_CURRENT_SOURCE_DIR}/${dir}
                $<TARGET_FILE_DIR:${PROJECT_NAME}>/${dir}
                COMMENT "Symlinking ./${dir} -> build/Debug/${dir}"
        )
    endforeach()
else()
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()
