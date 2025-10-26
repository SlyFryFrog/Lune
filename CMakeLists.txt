cmake_minimum_required(VERSION 4.0)
project(Lune LANGUAGES CXX)

#####################
# Compiler settings #
#####################

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

###########
# Options #
###########

set(ENABLE_CPP20_MODULE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

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

######################
# External libraries #
######################

find_package(glm CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(slang CONFIG REQUIRED)

################
# Vulkan setup #
################

find_package(Vulkan REQUIRED)

# set up Vulkan C++ module (enabled when ENABLE_CPP20_MODULE=ON)
add_library(VulkanCppModule)
add_library(Vulkan::cppm ALIAS VulkanCppModule)

target_compile_features(VulkanCppModule
        PRIVATE cxx_std_20
)

target_compile_definitions(VulkanCppModule PUBLIC
        VULKAN_HPP_NO_STRUCT_CONSTRUCTORS=1
)

target_include_directories(VulkanCppModule PRIVATE "${Vulkan_INCLUDE_DIR}")

target_link_libraries(VulkanCppModule PUBLIC Vulkan::Vulkan)

set_target_properties(VulkanCppModule PROPERTIES CXX_STANDARD 20)

target_sources(VulkanCppModule
        PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES
        BASE_DIRS "${Vulkan_INCLUDE_DIR}"
        FILES "${Vulkan_INCLUDE_DIR}/vulkan/vulkan.cppm"
)

# Require Vulkan version ≥ 1.3.256
if (${Vulkan_VERSION} VERSION_LESS "1.3.256")
    message(FATAL_ERROR
            "Minimum required Vulkan version for C++ modules is 1.3.256. "
            "Found ${Vulkan_VERSION}."
    )
endif ()

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

target_compile_features(Lune
        PRIVATE cxx_std_20
        INTERFACE cxx_std_20
)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        glm::glm
        glfw
        Vulkan::Vulkan
        Vulkan::cppm
        slang::gfx
        slang::slang
)

target_include_directories(${PROJECT_NAME}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor
)

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
endif()