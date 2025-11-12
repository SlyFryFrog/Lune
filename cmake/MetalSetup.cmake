function(setup_metal_for_target target_name)
    if (APPLE)
        # Define the Metal-CPP directory
        set(METAL_CPP_DIR "${CMAKE_SOURCE_DIR}/vendor/metal-cpp")

        # Download and extract Metal-CPP if it doesn't exist
        if (NOT EXISTS "${METAL_CPP_DIR}/Metal/Metal.hpp")
            message(STATUS "Downloading Metal-CPP...")

            # Download the ZIP file
            set(METAL_CPP_ZIP "${CMAKE_BINARY_DIR}/metal-cpp.zip")
            file(DOWNLOAD
                    "https://developer.apple.com/metal/cpp/files/metal-cpp_26.zip"
                    "${METAL_CPP_ZIP}"
                    SHOW_PROGRESS
            )

            # Extract the ZIP file
            message(STATUS "Extracting Metal-CPP...")
            execute_process(
                    COMMAND ${CMAKE_COMMAND} -E tar xzf "${METAL_CPP_ZIP}"
                    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/vendor"
            )

            # Rename the extracted directory to "metal-cpp" (if needed)
            # Note: The ZIP might extract to a directory like "metal-cpp_26". Adjust as needed.
            file(GLOB METAL_CPP_EXTRACTED_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendor/metal-cpp_*")
            if (METAL_CPP_EXTRACTED_DIR)
                file(RENAME "${METAL_CPP_EXTRACTED_DIR}" "${METAL_CPP_DIR}")
            endif ()

            # Clean up the ZIP file
            file(REMOVE "${METAL_CPP_ZIP}")
        endif ()

        target_include_directories(${target_name} PRIVATE ${METAL_CPP_DIR} ${METAL_CPP_EXT_DIR})

        # Find Metal frameworks
        find_library(COCOA_LIBRARY Cocoa)
        find_library(METAL_LIBRARY Metal)
        find_library(METAL_KIT_LIBRARY MetalKit)
        find_library(QUARTZ_CORE_LIBRARY QuartzCore)

        # Findd Metal extension frameworks
        find_library(APP_KIT_LIBRARY AppKit)
        find_library(METAL_FX_LIBRARY MetalFX)

        # Link Metal frameworks
        set(METAL_LIBRARIES
                ${COCOA_LIBRARY}
                ${METAL_LIBRARY}
                ${QUARTZ_CORE_LIBRARY}
                ${METAL_FX_LIBRARY}
        )
        target_link_libraries(${target_name} PRIVATE ${METAL_LIBRARIES})

        # Define USE_METAL for conditional compilation
        target_compile_definitions(${target_name}
                PRIVATE
                USE_METAL=1
        )
    else ()
        set(USE_METAL OFF CACHE BOOL "USE_METAL for rendering (Apple platforms only)" FORCE)
        message(WARNING "Metal is only supported on macOS. USE_METAL flag will be ignored.")
    endif ()
endfunction()
