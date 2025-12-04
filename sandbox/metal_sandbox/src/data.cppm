module;
export module sandbox:data;

import lune;

export namespace sandbox
{
	struct VertexData
	{
		lune::Vec4 position;
		lune::Vec2 color;
	};


	constexpr lune::Vec2 verticesA[] = {
		{-1.f, -1.f}, {1.f, -1.f}, {-1.f, 1.f},
		{-1.f, 1.f}, {1.f, -1.f}, {1.f, 1.f}
	};

	constexpr lune::Vec3 verticesB[] = {
		{-0.5f, -0.5f, 0.0f},
		{0.5f, -0.5f, 0.0f},
		{0.0f, 0.5f, 0.0f}
	};

	constexpr lune::Vec3 colors[] = {
		{1.0f, 0.0f, 0.0f}, // Red
		{0.0f, 1.0f, 0.0f}, // Green
		{0.0f, 0.0f, 1.0f}  // Blue
	};

	constexpr VertexData cubeVertices[] = {
		{{-0.5f, -0.5f, 0.5f, 1.0f}, {0.0f, 0.0f}}, // 0
		{{0.5f, -0.5f, 0.5f, 1.0f}, {1.0f, 0.0f}},  // 1
		{{0.5f, 0.5f, 0.5f, 1.0f}, {1.0f, 1.0f}},   // 2
		{{-0.5f, 0.5f, 0.5f, 1.0f}, {0.0f, 1.0f}},  // 3

		{{-0.5f, -0.5f, -0.5f, 1.0f}, {0.0f, 0.0f}}, // 4
		{{0.5f, -0.5f, -0.5f, 1.0f}, {1.0f, 0.0f}},  // 5
		{{0.5f, 0.5f, -0.5f, 1.0f}, {1.0f, 1.0f}},   // 6
		{{-0.5f, 0.5f, -0.5f, 1.0f}, {0.0f, 1.0f}},  // 7
	};

	constexpr unsigned int cubeIndices[] = {
		0, 1, 2, 0, 2, 3,
		1, 5, 6, 1, 6, 2,
		5, 4, 7, 5, 7, 6,
		4, 0, 3, 4, 3, 7,
		3, 2, 6, 3, 6, 7,
		4, 5, 1, 4, 1, 0
	};
}
