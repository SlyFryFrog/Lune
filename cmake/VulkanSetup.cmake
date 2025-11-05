function(setup_vulkan_for_target target_name)
    option(ENABLE_CPP20_MODULE "Enable C++20 modules" ON)

    find_package(Vulkan REQUIRED)

    # Vulkan C++ module
    add_library(VulkanCppModule)
    add_library(Vulkan::cppm ALIAS VulkanCppModule)
    target_compile_features(VulkanCppModule PRIVATE cxx_std_20)
    target_compile_definitions(VulkanCppModule PUBLIC VULKAN_HPP_NO_STRUCT_CONSTRUCTORS=1)
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
        message(FATAL_ERROR "Minimum required Vulkan version for C++ modules is 1.3.256. Found ${Vulkan_VERSION}.")
    endif ()

    target_link_libraries(${target_name} PRIVATE Vulkan::Vulkan Vulkan::cppm)
endfunction()
