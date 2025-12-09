import lune;
import sandbox;
using namespace sandbox;

lune::Timer timer;


int main()
{
	lune::setWorkingDirectory();

	const lune::raii::Window window{{
		.width = 1280,
		.height = 720,
		.title = "Lune: Sandbox - Metal Renderer",
		.resizable = true,
	}};

	auto& context{lune::gfx::GraphicsContext::instance()};

	lune::metal::GraphicsShader shader{"shaders/cube.metal"};
	lune::metal::GraphicsPipeline pipeline{shader};
	lune::metal::RenderPass pass{window.surface()};

	lune::metal::Material material{pipeline};
	material.setUniform("cubeData", cubeVertices);

	lune::gfx::Buffer indexBuffer{context.createBuffer(sizeof(cubeIndices))};
	indexBuffer.update(cubeIndices, sizeof(cubeIndices));

	// Create our first shader
	lune::metal::GraphicsShader shaderA{"shaders/basic.metal"};
	lune::metal::GraphicsPipeline pipelineA{shaderA};

	lune::gfx::Texture texture{context.createTexture({})};
	texture.load("shaders/img.png");

	lune::metal::Material materialA{pipelineA};
	materialA.setUniform("verts", verticesA).setUniform("tex", texture);

	// Create our second shader
	lune::metal::GraphicsShader shaderA2{"shaders/basic2.metal"};
	lune::metal::GraphicsPipeline pipelineA2{shaderA2};
	lune::metal::Material materialA2{pipelineA2};
	materialA2.setUniform("vertexPositions", verticesB).setUniform("vertexColors", colors);


	window.show();
	timer.start();
	while (!window.shouldClose())
	{
		if (lune::InputManager::isJustPressed(lune::KEY_ESCAPE))
			window.setShouldClose(true);
		material.setUniform("u", static_cast<float>(timer.peakDelta()));

		if (lune::InputManager::isPressed(lune::KEY_W))
		{
			pass.begin()
				.setFillMode(lune::gfx::Wireframe)
				.bind(material)
				.drawIndexed(lune::gfx::Triangle, 36, indexBuffer) // Optimized indexed draw
				.bind(materialA2)
				.draw(lune::gfx::Triangle, 0, 3)
				.bind(materialA)
				.draw(lune::gfx::Triangle, 0, 6)
				.end();
		}
		else
		{
			pass.begin()
				.bind(material)
				.drawIndexed(lune::gfx::Triangle, 36, indexBuffer)
				.bind(materialA2)
				.draw(lune::gfx::Triangle, 0, 3)
				.bind(materialA)
				.draw(lune::gfx::Triangle, 0, 6)
				.end();
		}


		lune::Window::pollEvents();
	}

	return 0;
}
