#include <Metal/Metal.hpp>
#include <iostream>
#include <vector>
#include <cmath>
import lune;

constexpr size_t arrayLength = 1 << 26;
constexpr size_t iterations = 10;

void cpuAddMultiply(const std::vector<float>& a,
                    const std::vector<float>& b,
                    std::vector<float>& addOut,
                    std::vector<float>& mulOut)
{
	for (size_t i = 0; i < a.size(); ++i)
	{
		addOut[i] = a[i] + b[i];
		mulOut[i] = a[i] * b[i];
	}
}

void printProgress(const size_t current, const size_t total)
{
	const float ratio = static_cast<float>(current) / static_cast<float>(total);
	constexpr int width = 50;
	const int filled = static_cast<int>(ratio * width);

	std::cout << "\r[";
	for (int i = 0; i < filled; i++)
		std::cout << '=';
	for (int i = filled; i < width; i++)
		std::cout << ' ';
	std::cout << "] " << static_cast<int>(ratio * 100.0f) << "%";
	if (current == total)
		std::cout << "\n";
	std::cout.flush();
}


int main()
{
	lune::setWorkingDirectory();

	// CPU input data
	std::vector<float> a(arrayLength), b(arrayLength), outputAdd(arrayLength), outputMul(arrayLength);
	for (size_t i = 0; i < arrayLength; ++i)
	{
		a[i] = static_cast<float>(rand()) / RAND_MAX;
		b[i] = static_cast<float>(rand()) / RAND_MAX;
	}

	// Create shader
	lune::metal::ComputeShaderCreateInfo info{
		.path = "shaders/basic.metal",
		.kernels = {"add_arrays", "multiply_arrays"}
	};

	// Add our buffers to our shader
	auto shader = lune::metal::ComputeShader(info);
	auto kernelAdd = shader.kernel("add_arrays")
	                       .setBuffer("inputA", a)
	                       .setBuffer("inputB", b)
	                       .setBuffer("outputAdd", outputAdd);
	auto kernelMul = shader.kernel("multiply_arrays")
	                       .setBuffer("inputA", a)
	                       .setBuffer("inputB", b)
	                       .setBuffer("outputMul", outputMul);

	printKernelInfo(kernelAdd.reflection());
	printKernelInfo(kernelMul.reflection());

	// Dispatch GPU
	lune::Timer timer;
	timer.start();
	for (int i = 0; i < iterations; ++i)
	{
		printProgress(i, iterations);
		kernelAdd.dispatch(arrayLength);
		kernelMul.dispatch(arrayLength);
	}
	printProgress(iterations, iterations);


	kernelAdd.waitUntilComplete();
	kernelMul.waitUntilComplete();

	auto outA = kernelAdd.buffer("outputAdd");
	auto outB = kernelMul.buffer("outputMul");

	memcpy(outputAdd.data(), outA->contents(), arrayLength * sizeof(float));
	memcpy(outputMul.data(), outB->contents(), arrayLength * sizeof(float));

	auto duration = timer.delta() * 1000;
	std::cout << "GPU add/multiply time: " << duration << " ms\n";

	// Verify CPU vs GPU
	std::vector<float> addOut(arrayLength), mulOut(arrayLength);
	timer.start();
	for (int i = 0; i < iterations; ++i)
	{
		printProgress(i, iterations);
		cpuAddMultiply(a, b, addOut, mulOut);
	}
	printProgress(iterations, iterations);
	duration = timer.delta() * 1000;
	std::cout << "CPU add/multiply time: " << duration << " ms\n";

	bool correct = true;
	for (size_t i = 0; i < arrayLength; ++i)
	{
		if (std::abs(outputAdd[i] - addOut[i]) > 1e-5f || std::abs(outputMul[i] - mulOut[i]) > 1e-5f)
		{
			correct = false;
			std::cerr << "Mismatch at index " << i << ": GPU(" << outputAdd[i] << "," << outputMul[i]
				<< ") vs CPU(" << addOut[i] << "," << mulOut[i] << ")\n";
			break;
		}
	}
	if (correct)
		std::cout << "GPU results verified!\n";

	return 0;
}
