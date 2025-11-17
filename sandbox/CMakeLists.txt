add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/hello_window)

if (USE_METAL)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/metal_hello_triangle)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/metal_sandbox)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/metal_compute_basic)
endif ()