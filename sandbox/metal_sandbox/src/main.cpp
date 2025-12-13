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

	auto& ctx{lune::gfx::Context::instance()};

	lune::gfx::Shader shader{ctx.createShader({"shaders/cube.metal"})};
	lune::gfx::Pipeline pipeline{ctx.createPipeline(shader, {})};
	lune::gfx::RenderPass pass{ctx.createRenderPass(window.surface())};

	lune::gfx::Material material{ctx.createMaterial(pipeline)};
	material.setUniform("cubeData", cubeVertices, sizeof(cubeVertices));

	lune::gfx::Buffer indexBuffer{ctx.createBuffer(sizeof(cubeIndices))};
	indexBuffer.setData(cubeIndices, sizeof(cubeIndices));

	// Create our first shader
	lune::gfx::Shader shaderA{ctx.createShader({"shaders/basic.metal"})};
	lune::gfx::Pipeline pipelineA{ctx.createPipeline(shaderA, {})};

	lune::gfx::Texture texture{ctx.createTexture({})};
	texture.load("shaders/img.png");

	lune::gfx::Material materialA{ctx.createMaterial(pipelineA)};
	materialA.setUniform("verts", verticesA, sizeof(verticesA)).setUniform("tex", texture);

	// Create our second shader
	lune::gfx::Shader shaderA2{ctx.createShader({"shaders/basic2.metal"})};
	lune::gfx::Pipeline pipelineA2{ctx.createPipeline(shaderA2, {})};
	lune::gfx::Material materialA2{ctx.createMaterial(pipelineA2)};
	materialA2.setUniform("vertexPositions", verticesB, sizeof(verticesB))
			.setUniform("vertexColors", colors, sizeof(colors));


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
					.drawIndexed(lune::gfx::Triangle, 36, indexBuffer, 0) // Optimized indexed draw
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
					.drawIndexed(lune::gfx::Triangle, 36, indexBuffer, 0)
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
