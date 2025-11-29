/**
 * @note We can't directly expose simd::float4x4, etc. in function definitions
 * and therefore have to use simd_float4x4, etc.
 */

module;
#include <cmath>
#include <cstddef>
#ifdef LUNE_USE_SIMD
#include <simd/matrix_types.h>
#elif defined(LUNE_USE_GLM)
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#endif
export module lune:matrix;

import :vector;

export namespace lune
{
	struct alignas(16) Mat3
	{
		float m[3][3]{};

		constexpr Mat3() noexcept = default;

		constexpr explicit Mat3(const float diag) noexcept
		{
			m[0][0] = diag;
			m[1][1] = diag;
			m[2][2] = diag;
			m[0][1] = m[0][2] = m[1][0] = m[1][2] = m[2][0] = m[2][1] = 0;
		}

		constexpr Mat3(const float m00, const float m01, const float m02,
		               const float m10, const float m11, const float m12,
		               const float m20, const float m21, const float m22) noexcept :
			m{{m00, m01, m02},
			  {m10, m11, m12},
			  {m20, m21, m22,}}
		{
		}

		constexpr Mat3 operator+(const Mat3& o) const noexcept
		{
			Mat3 r{};
			for (int i = 0; i < 3; ++i)
				for (int j = 0; j < 3; ++j)
					r.m[i][j] = m[i][j] + o.m[i][j];
			return r;
		}

		constexpr Mat3 operator-(const Mat3& o) const noexcept
		{
			Mat3 r{};
			for (int i = 0; i < 3; ++i)
				for (int j = 0; j < 3; ++j)
					r.m[i][j] = m[i][j] - o.m[i][j];
			return r;
		}

		constexpr Mat3 operator*(const float s) const noexcept
		{
			Mat3 r{};
			for (int i = 0; i < 3; ++i)
				for (int j = 0; j < 3; ++j)
					r.m[i][j] = m[i][j] * s;
			return r;
		}

		constexpr Mat3 operator*(const Mat3& o) const noexcept
		{
			Mat3 r{};
			for (int i = 0; i < 3; ++i)
				for (int j = 0; j < 3; ++j)
					for (int k = 0; k < 3; ++k)
						r.m[i][j] += m[i][k] * o.m[k][j];
			return r;
		}

		constexpr Mat3 operator/(const Mat3& o) const noexcept
		{
			Mat3 r{};
			for (int i = 0; i < 3; ++i)
				for (int j = 0; j < 3; ++j)
					r.m[i][j] = m[i][j] / o.m[i][j];
			return r;
		}

		constexpr float* operator[](const size_t row) noexcept
		{
			return m[row];
		}

		constexpr const float* operator[](const size_t row) const noexcept
		{
			return m[row];
		}

		[[nodiscard]] static constexpr Mat3 identity() noexcept
		{
			return Mat3{1.0f};
		}

#ifdef LUNE_USE_SIMD
		constexpr explicit Mat3(const simd_float3x3& v) noexcept :
			m({v.columns[0][0], v.columns[0][1], v.columns[0][2]},
			  {v.columns[1][0], v.columns[1][1], v.columns[1][2]},
			  {v.columns[2][0], v.columns[2][1], v.columns[2][2]})
		{
		}

		constexpr explicit operator simd_float3x3() const noexcept
		{
			return simd_float3x3{
				simd_make_float3(m[0][0], m[0][1], m[0][2]),
				simd_make_float3(m[1][0], m[1][1], m[1][2]),
				simd_make_float3(m[2][0], m[2][1], m[2][2])
			};
		}
#endif
	};


	struct alignas(16) Mat4
	{
		float m[4][4]{};

		constexpr Mat4() noexcept = default;

		constexpr explicit Mat4(const float diag) noexcept
		{
			for (int i = 0; i < 4; ++i)
				for (int j = 0; j < 4; ++j)
					m[i][j] = (i == j ? diag : 0);
		}

		constexpr Mat4(const float m00, const float m01, const float m02, const float m03,
		               const float m10, const float m11, const float m12, const float m13,
		               const float m20, const float m21, const float m22, const float m23,
		               const float m30, const float m31, const float m32, const float m33
			) noexcept :
			m{{m00, m01, m02, m03},
			  {m10, m11, m12, m13},
			  {m20, m21, m22, m23},
			  {m30, m31, m32, m33}}
		{
		}

		constexpr Mat4 operator+(const Mat4& o) const noexcept
		{
			Mat4 r{};
			for (int i = 0; i < 4; ++i)
				for (int j = 0; j < 4; ++j)
					r.m[i][j] = m[i][j] + o.m[i][j];
			return r;
		}

		constexpr Mat4 operator-(const Mat4& o) const noexcept
		{
			Mat4 r{};
			for (int i = 0; i < 4; ++i)
				for (int j = 0; j < 4; ++j)
					r.m[i][j] = m[i][j] - o.m[i][j];
			return r;
		}

		constexpr Mat4 operator*(const float s) const noexcept
		{
			Mat4 r{};
			for (int i = 0; i < 4; ++i)
				for (int j = 0; j < 4; ++j)
					r.m[i][j] = m[i][j] * s;
			return r;
		}

		constexpr Mat4 operator*(const Mat4& o) const noexcept
		{
			Mat4 r{};
			for (int i = 0; i < 4; ++i)
				for (int j = 0; j < 4; ++j)
					for (int k = 0; k < 4; ++k)
						r.m[i][j] += m[i][k] * o.m[k][j];
			return r;
		}

		constexpr Mat4 operator/(const Mat4& o) const noexcept
		{
			Mat4 r{};
			for (int i = 0; i < 4; ++i)
				for (int j = 0; j < 4; ++j)
					r.m[i][j] = m[i][j] / o.m[i][j];
			return r;
		}

		constexpr float* operator[](const size_t row) noexcept
		{
			return m[row];
		}

		constexpr const float* operator[](const size_t row) const noexcept
		{
			return m[row];
		}

		[[nodiscard]] constexpr Mat4 transpose(const Mat4& o) const noexcept
		{
			return Mat4{
				m[0][0], m[1][0], m[2][0], m[3][0],
				m[0][1], m[1][1], m[2][1], m[3][1],
				m[0][2], m[1][2], m[2][2], m[3][2],
				m[0][3], m[1][3], m[2][3], m[3][3]
			};
		}

		static constexpr Mat4 identity() noexcept
		{
			return Mat4(1.0f);
		}

		static constexpr Mat4 zero() noexcept
		{
			return Mat4(0.0f);
		}

		[[nodiscard]] static constexpr Mat4 translate(const float x, const float y,
		                                              const float z) noexcept
		{
			Mat4 r = identity();
			r.m[3][0] = x;
			r.m[3][1] = y;
			r.m[3][2] = z;
			return r;
		}

		static constexpr Mat4 rotate(const float angle, const Vec3& axis) noexcept
		{
			const float c = std::cos(angle);
			const float s = std::sin(angle);
			const float omc = 1.0f - c;
			const Vec3 normAxis = axis.normalize();

			return Mat4{
				c + normAxis.x * normAxis.x * omc,
				normAxis.x * normAxis.y * omc - normAxis.z * s,
				normAxis.x * normAxis.z * omc + normAxis.y * s,
				0.0f,

				normAxis.y * normAxis.x * omc + normAxis.z * s,
				c + normAxis.y * normAxis.y * omc,
				normAxis.y * normAxis.z * omc - normAxis.x * s,
				0.0f,

				normAxis.z * normAxis.x * omc - normAxis.y * s,
				normAxis.z * normAxis.y * omc + normAxis.x * s,
				c + normAxis.z * normAxis.z * omc,
				0.0f,

				0.0f, 0.0f, 0.0f, 1.0f
			};
		}

		static constexpr Mat4 scale(const float x, const float y, const float z) noexcept
		{
			return Mat4{
				x, 0.0f, 0.0f, 0.0f,
				0.0f, y, 0.0f, 0.0f,
				0.0f, 0.0f, z, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
			};
		}

		static constexpr Mat4 perspective(const float fov, const float aspect, const float near,
		                                  const float far) noexcept
		{
			const float tan_half_fov = std::tan(fov * 0.5f);
			return Mat4{
				1.0f / (aspect * tan_half_fov), 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f / tan_half_fov, 0.0f, 0.0f,
				0.0f, 0.0f, -(far + near) / (far - near), -1.0f,
				0.0f, 0.0f, -(2.0f * far * near) / (far - near), 0.0f
			};
		}

#ifdef LUNE_USE_SIMD
		constexpr explicit Mat4(const simd_float4x4& v) noexcept :
			m({v.columns[0][0], v.columns[0][1], v.columns[0][2], v.columns[0][3]},
			  {v.columns[1][0], v.columns[1][1], v.columns[1][2], v.columns[1][3]},
			  {v.columns[2][0], v.columns[2][1], v.columns[2][2], v.columns[2][3]},
			  {v.columns[3][0], v.columns[3][1], v.columns[3][2], v.columns[3][3]})
		{
		}

		constexpr explicit operator simd_float4x4() const noexcept
		{
			return simd_float4x4{
				simd_make_float4(m[0][0], m[0][1], m[0][2], m[0][3]),
				simd_make_float4(m[1][0], m[1][1], m[1][2], m[1][3]),
				simd_make_float4(m[2][0], m[2][1], m[2][2], m[2][3]),
				simd_make_float4(m[3][0], m[3][1], m[3][2], m[3][3])
			};
		}
#endif
	};
}
