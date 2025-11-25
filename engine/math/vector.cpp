module;
#include <simd/vector_types.h>
module lune;

namespace lune
{
#ifdef LUNE_USE_SIMD
	constexpr Vec2::Vec2(const simd::float2& v) noexcept :
		x(v.x),
		y(v.y)
	{
	}

	constexpr Vec2::operator simd::float2() const noexcept
	{
		return {x, y};
	}

	constexpr Vec3::Vec3(const simd::float3& v) noexcept :
		x(v.x),
		y(v.y),
		z(v.z)
	{
	}

	constexpr Vec3::operator simd::float3() const noexcept
	{
		return {x, y, z};
	}
#endif
}
