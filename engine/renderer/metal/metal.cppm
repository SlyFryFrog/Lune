module;
#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION

#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
export module lune.metal;

export import :buffer;
export import :compute;
export import :context;
export import :mappings;
export import :render_surface;
export import :texture;
export import :objc_interop;
export import :graphics;
