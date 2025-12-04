#include <iostream>
import lune;
import sandbox;
using namespace sandbox;

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
	lune::metal::RenderPass pass{window.surface()};

	lune::metal::Material material{pipeline};
	material.setUniform("cubeData", cubeVertices);

	lune::Buffer indexBuffer = context.createBuffer(sizeof(cubeIndices));
	indexBuffer.update(cubeIndices, sizeof(cubeIndices));

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
				pass.begin()
				    .bind(material)
				    .setFillMode(lune::Wireframe)
				    .setCullMode(lune::None)
				    .draw(lune::Triangle, 36, indexBuffer) // Optimized indexed draw
				    .end();
			}
			else
			{
				pass.begin()
				    .bind(material)
				    .draw(lune::Triangle, 36, indexBuffer)
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
	lune::metal::RenderPass pass{window.surface()};

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
	pass.begin()
	    .bind(material2)
	    .draw(lune::Triangle, 0, 3)
	    .bind(materialA)
	    .draw(lune::Triangle, 0, 6)
	    .end();
}
