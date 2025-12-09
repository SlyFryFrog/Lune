module;
#ifdef LUNE_USE_SIMD
#include <simd/vector_types.h>
#elif defined(LUNE_USE_GLM)
#warning LUNE_USE_GLM was declared, however this implementation is incomplete and may not work.
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#endif
module lune;

namespace lune
{
#ifdef LUNE_USE_SIMD
	constexpr Vec2::Vec2(const simd::float2& v) noexcept : x(v.x), y(v.y)
	{
	}

	constexpr Vec2::operator simd::float2() const noexcept
	{
		return {x, y};
	}

	constexpr Vec3::Vec3(const simd::float3& v) noexcept : x(v.x), y(v.y), z(v.z)
	{
	}

	constexpr Vec3::operator simd::float3() const noexcept
	{
		return {x, y, z};
	}

	constexpr Vec4::Vec4(const simd::float4& v) noexcept : x(v.x), y(v.y), z(v.z), w(v.w)
	{
	}

	constexpr Vec4::operator simd::float4() const noexcept
	{
		return {x, y, z, w};
	}
#endif
} // namespace lune
