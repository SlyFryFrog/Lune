/**
* @note SIMD-specific implementations are separated into a source/implementation file
 *       to prevent breaking IntelliSense when parsing the module interface.
 */

module;
#include <cmath>

#ifdef LUNE_USE_SIMD
#include <simd/vector_types.h>
#elif defined(LUNE_USE_GLM)
#warning LUNE_USE_GLM was declared, however this implementation is incomplete and may not work.
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
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

		constexpr Vec2 operator+(const Vec2& o) const noexcept
		{
			return {x + o.x, y + o.y};
		}

		constexpr Vec2 operator-(const Vec2& o) const noexcept
		{
			return {x - o.x, y - o.y};
		}

		constexpr Vec2 operator*(const float s) const noexcept
		{
			return {x * s, y * s};
		}

		constexpr Vec2 operator/(const float s) const noexcept
		{
			return {x / s, y / s};
		}

		[[nodiscard]] constexpr float dot(const Vec2& o) const noexcept
		{
			return x * o.x + y * o.y;
		}

		[[nodiscard]] float length() const noexcept
		{
			return std::sqrt(dot(*this));
		}

		[[nodiscard]] Vec2 normalize() const noexcept
		{
			const float len = length();
			return len == 0 ? Vec2{} : *this / len;
		}

#ifdef LUNE_USE_SIMD
		constexpr explicit Vec2(const simd::float2& v) noexcept;
		constexpr explicit operator simd::float2() const noexcept;
#endif

#ifdef LUNE_USE_GLM
		constexpr explicit Vec2(const glm::vec2& v) noexcept;
		constexpr operator glm::vec2() const noexcept;
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

		constexpr Vec3 operator+(const Vec3& o) const noexcept
		{
			return {x + o.x, y + o.y, z + o.z};
		}

		constexpr Vec3 operator-(const Vec3& o) const noexcept
		{
			return {x - o.x, y - o.y, z - o.z};
		}

		constexpr Vec3 operator*(const float s) const noexcept
		{
			return {x * s, y * s, z * s};
		}

		constexpr Vec3 operator/(const float s) const noexcept
		{
			return {x / s, y / s, z / s};
		}

		[[nodiscard]] constexpr float dot(const Vec3& o) const noexcept
		{
			return x * o.x + y * o.y + z * o.z;
		}

		[[nodiscard]] constexpr Vec3 cross(const Vec3& o) const noexcept
		{
			return {y * o.z - z * o.y,
			        z * o.x - x * o.z,
			        x * o.y - y * o.x};
		}

		[[nodiscard]] float length() const noexcept
		{
			return std::sqrt(dot(*this));
		}

		[[nodiscard]] Vec3 normalize() const noexcept
		{
			const float len = length();
			return len == 0 ? Vec3{} : *this / len;
		}

#ifdef LUNE_USE_SIMD
		constexpr explicit Vec3(const simd::float3& v) noexcept;
		constexpr explicit operator simd::float3() const noexcept;
#endif

#ifdef LUNE_USE_GLM
		constexpr explicit Vec3(const glm::vec3& v) noexcept;
		constexpr operator glm::vec3() const noexcept;
#endif
	};


	struct alignas(16) Vec4
	{
		float x, y, z, w;

		constexpr Vec4() noexcept :
			x(0),
			y(0),
			z(0),
			w(0)
		{
		}

		constexpr Vec4(const float X, const float Y, const float Z, const float W) noexcept :
			x(X),
			y(Y),
			z(Z),
			w(W)
		{
		}

		constexpr Vec4 operator+(const Vec4& o) const noexcept
		{
			return {x + o.x, y + o.y, z + o.z, w + o.w};
		}

		constexpr Vec4 operator-(const Vec4& o) const noexcept
		{
			return {x - o.x, y - o.y, z - o.z, w - o.w};
		}

		constexpr Vec4 operator*(const float s) const noexcept
		{
			return {x * s, y * s, z * s, w * s};
		}

		constexpr Vec4 operator/(const float s) const noexcept
		{
			return {x / s, y / s, z / s, w / s};
		}

		[[nodiscard]] constexpr float dot(const Vec4& o) const noexcept
		{
			return x * o.x + y * o.y + z * o.z + w * o.w;
		}

		[[nodiscard]] float length() const noexcept
		{
			return std::sqrt(dot(*this));
		}

		[[nodiscard]] Vec4 normalize() const noexcept
		{
			const float len = length();
			return len == 0 ? Vec4{} : *this / len;
		}

#ifdef LUNE_USE_SIMD
		constexpr explicit Vec4(const simd::float4& v) noexcept;
		constexpr explicit operator simd::float4() const noexcept;
#endif

#ifdef LUNE_USE_GLM
		constexpr explicit Vec4(const glm::vec4& v) noexcept;
		constexpr operator glm::vec4() const noexcept;
#endif
	};
}
