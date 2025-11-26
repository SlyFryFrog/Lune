module;
#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION

#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
export module lune:metal;

export import :metal_compute;
export import :metal_context;
export import :metal_mappings;
export import :metal_shader;
export import :metal_texture;
export import :objc_interop;
