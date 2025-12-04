import lune;

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

constexpr VertexData cubeData[] = {
	{{-0.5, -0.5, 0.5, 1.0}, {0.0, 0.0}},
	{{0.5, -0.5, 0.5, 1.0}, {1.0, 0.0}},
	{{0.5, 0.5, 0.5, 1.0}, {1.0, 1.0}},
	{{0.5, 0.5, 0.5, 1.0}, {1.0, 1.0}},
	{{-0.5, 0.5, 0.5, 1.0}, {0.0, 1.0}},
	{{-0.5, -0.5, 0.5, 1.0}, {0.0, 0.0}},

	// Back face
	{{0.5, -0.5, -0.5, 1.0}, {0.0, 0.0}},
	{{-0.5, -0.5, -0.5, 1.0}, {1.0, 0.0}},
	{{-0.5, 0.5, -0.5, 1.0}, {1.0, 1.0}},
	{{-0.5, 0.5, -0.5, 1.0}, {1.0, 1.0}},
	{{0.5, 0.5, -0.5, 1.0}, {0.0, 1.0}},
	{{0.5, -0.5, -0.5, 1.0}, {0.0, 0.0}},

	// Top face
	{{-0.5, 0.5, 0.5, 1.0}, {0.0, 0.0}},
	{{0.5, 0.5, 0.5, 1.0}, {1.0, 0.0}},
	{{0.5, 0.5, -0.5, 1.0}, {1.0, 1.0}},
	{{0.5, 0.5, -0.5, 1.0}, {1.0, 1.0}},
	{{-0.5, 0.5, -0.5, 1.0}, {0.0, 1.0}},
	{{-0.5, 0.5, 0.5, 1.0}, {0.0, 0.0}},

	// Bottom face
	{{-0.5, -0.5, -0.5, 1.0}, {0.0, 0.0}},
	{{0.5, -0.5, -0.5, 1.0}, {1.0, 0.0}},
	{{0.5, -0.5, 0.5, 1.0}, {1.0, 1.0}},
	{{0.5, -0.5, 0.5, 1.0}, {1.0, 1.0}},
	{{-0.5, -0.5, 0.5, 1.0}, {0.0, 1.0}},
	{{-0.5, -0.5, -0.5, 1.0}, {0.0, 0.0}},

	// Left face
	{{-0.5, -0.5, -0.5, 1.0}, {0.0, 0.0}},
	{{-0.5, -0.5, 0.5, 1.0}, {1.0, 0.0}},
	{{-0.5, 0.5, 0.5, 1.0}, {1.0, 1.0}},
	{{-0.5, 0.5, 0.5, 1.0}, {1.0, 1.0}},
	{{-0.5, 0.5, -0.5, 1.0}, {0.0, 1.0}},
	{{-0.5, -0.5, -0.5, 1.0}, {0.0, 0.0}},

	// Right face
	{{0.5, -0.5, 0.5, 1.0}, {0.0, 0.0}},
	{{0.5, -0.5, -0.5, 1.0}, {1.0, 0.0}},
	{{0.5, 0.5, -0.5, 1.0}, {1.0, 1.0}},
	{{0.5, 0.5, -0.5, 1.0}, {1.0, 1.0}},
	{{0.5, 0.5, 0.5, 1.0}, {0.0, 1.0}},
	{{0.5, -0.5, 0.5, 1.0}, {0.0, 0.0}},
};

lune::Timer timer;
const lune::raii::Window window{{
	.width = 1280,
	.height = 720,
	.title = "Lune: Sandbox - Metal Renderer",
	.resizable = true,
}};

void drawLayeredDemo(lune::GraphicsContext& context);
void drawCube(lune::GraphicsContext& context);


int main()
{
	lune::setWorkingDirectory();
	auto& context = lune::GraphicsContext::instance();

	lune::metal::GraphicsShader shader{"shaders/cube.metal"};
	lune::metal::GraphicsPipeline pipeline{shader};
	lune::metal::RenderPass pass{};

	lune::metal::Material material{pipeline};
	material.setUniform("cubeData", cubeData);

	window.show();
	timer.start();
	while (!window.shouldClose())
	{
		if (lune::InputManager::isJustPressed(lune::KEY_ESCAPE))
			window.setShouldClose(true);
		if (lune::InputManager::isJustPressed(lune::KEY_1))
			drawLayeredDemo(context);
		if (lune::InputManager::isPressed(lune::KEY_2))
		{
			material.setUniform("u", static_cast<float>(timer.peakDelta()));

			if (lune::InputManager::isPressed(lune::KEY_W))
			{
				pass.begin(window.surface())
				    .bind(material)
				    .setFillMode(lune::Wireframe)
				    .setCullMode(lune::None) // Disable optimization
				    .draw(lune::Triangle, 0, 36)
				    .end();
			}
			else
			{
				pass.begin(window.surface())
				    .bind(material)
				    .draw(lune::Triangle, 0, 36)
				    .end();
			}
		}

		lune::Window::pollEvents();
	}

	return 0;
}


void drawLayeredDemo(lune::GraphicsContext& context)
{
	// Create our first shader
	lune::metal::GraphicsShader shader{"shaders/basic.metal"};
	lune::metal::GraphicsPipeline pipeline{shader};
	lune::metal::RenderPass pass{};

	lune::Texture texture = context.createTexture({});
	texture.load("shaders/img.png");

	lune::metal::Material materialA{pipeline};
	materialA.setUniform("verts", verticesA)
	         .setUniform("tex", texture);

	// Create our second shader
	lune::metal::GraphicsShader shader2{"shaders/basic2.metal"};
	lune::metal::GraphicsPipeline pipeline2{shader2};
	lune::metal::Material material2{pipeline2};
	material2.setUniform("vertexPositions", verticesB)
	         .setUniform("vertexColors", colors);

	// First drawn is placed on top
	pass.begin(window.surface())
	    .bind(material2)
	    .draw(lune::Triangle, 0, 3)
	    .bind(materialA)
	    .draw(lune::Triangle, 0, 6)
	    .end();
}
