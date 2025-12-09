set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

cmake_minimum_required(VERSION 4.0 FATAL_ERROR)
project(Lune LANGUAGES CXX)

if (APPLE AND USE_METAL)
    if (USE_VULKAN)
        set(USE_VULKAN OFF CACHE BOOL "Deactivating USE_VULKAN as USE_METAL was specified" FORCE)
    endif ()

    include(cmake/MetalSetup.cmake)
else ()
    if (USE_METAL)
        set(USE_METAL OFF CACHE BOOL "USE_METAL for rendering (Apple platforms only)" FORCE)
        set(USE_VULKAN ON CACHE BOOL "Defaulting to USE_VULKAN" FORCE)
        message(WARNING "Metal is only supported on macOS. USE_METAL flag will be ignored.")
    endif ()
    include(cmake/VulkanSetup.cmake)
endif ()

#####################
# Compiler settings #
#####################
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

###########
# Options #
###########
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#################################
# Set constants for the project #
#################################
file(GLOB_RECURSE SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/engine/*.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/engine/*.cpp
)
file(GLOB_RECURSE MODULES
        ${CMAKE_CURRENT_SOURCE_DIR}/engine/*.cppm
)

# Remove unused backend renderer
if (USE_METAL)
    file(GLOB VULKAN_SOURCES
            ${CMAKE_CURRENT_SOURCE_DIR}/engine/renderer/vulkan/*.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/engine/renderer/vulkan/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/engine/renderer/vulkan/*.cppm
    )
    list(REMOVE_ITEM SOURCES ${VULKAN_SOURCES})
    list(REMOVE_ITEM MODULES ${VULKAN_SOURCES})
else ()
    file(GLOB METAL_SOURCES
            ${CMAKE_CURRENT_SOURCE_DIR}/engine/renderer/metal/*.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/engine/renderer/metal/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/engine/renderer/metal/*.cppm
    )
    list(REMOVE_ITEM SOURCES ${METAL_SOURCES})
    list(REMOVE_ITEM MODULES ${METAL_SOURCES})
endif ()

######################
# External libraries #
######################
find_package(glfw3 CONFIG REQUIRED)

add_library(stb_image STATIC ${CMAKE_CURRENT_SOURCE_DIR}/vendor/stb_image.cpp)

###################
# Target: library #
###################
add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_sources(${PROJECT_NAME}
        PUBLIC
        FILE_SET allModules
        TYPE CXX_MODULES
        FILES ${MODULES}
)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20 INTERFACE cxx_std_20)
target_link_libraries(${PROJECT_NAME}
        PRIVATE
        glfw
        stb_image
)

target_include_directories(${PROJECT_NAME}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor
)

################################
# Add math conversions support #
################################
if(LUNE_USE_SIMD)
    target_compile_definitions(${PROJECT_NAME} PRIVATE LUNE_USE_SIMD)
endif()

if(LUNE_USE_GLM)
    target_compile_definitions(${PROJECT_NAME} PRIVATE LUNE_USE_GLM)
    find_package(glm CONFIG REQUIRED)
    target_link_libraries(${PROJECT_NAME} glm::glm)
endif()

##################
# Renderer setup #
##################
if (USE_METAL)
    setup_metal_for_target(${PROJECT_NAME})
else ()
    setup_vulkan_for_target(${PROJECT_NAME})
endif ()

###############################
# Compile commands for clangd #
###############################
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_BINARY_DIR}/compile_commands.json
            ${CMAKE_SOURCE_DIR}/compile_commands.json
    )
endif ()

############################
# Compile sandbox projects #
############################
if (BUILD_SANDBOX)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/sandbox)
endif ()

#######################
# Build project tests #
#######################
if (LUNE_TESTS)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)
endif ()