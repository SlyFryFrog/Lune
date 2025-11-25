/**
* @note SIMD-specific implementations are separated into a source/implementation file
 *       to prevent breaking IntelliSense when parsing the module interface.
 */

module;
#ifdef LUNE_USE_SIMD
#include <simd/vector_types.h>
#endif
export module lune:vector;


export namespace lune
{
	struct alignas(8) Vec2
	{
		float x, y;

		constexpr Vec2() noexcept :
			x(0),
			y(0)
		{
		}

		constexpr Vec2(const float X, const float Y) noexcept :
			x(X),
			y(Y)
		{
		}

#ifdef LUNE_USE_SIMD
		constexpr explicit Vec2(const simd::float2& v) noexcept;
		constexpr explicit operator simd::float2() const noexcept;
#endif
	};


	struct alignas(16) Vec3
	{
		float x, y, z;
		float _pad = 0;

		constexpr Vec3() noexcept :
			x(0),
			y(0),
			z(0)
		{
		}

		constexpr Vec3(const float X, const float Y, const float Z) noexcept :
			x(X),
			y(Y),
			z(Z)
		{
		}

#ifdef LUNE_USE_SIMD
		constexpr explicit Vec3(const simd::float3& v) noexcept;
		constexpr explicit operator simd::float3() const noexcept;
#endif
	};
}
