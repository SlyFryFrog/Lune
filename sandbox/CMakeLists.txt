add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/hello_triangle)

if (USE_METAL)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/metal_hello_triangle)
endif ()