#include <Metal/Metal.hpp>
#include <iostream>
#include <vector>
#include <cmath>
import lune;

constexpr size_t arrayLength = 1 << 24;
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

void printKernelInfo(const lune::metal::KernelReflectionInfo& info)
{
	std::cout << "================= Kernel Reflection =================\n";
	std::cout << "Function: " << info.kernelName << "\n\n";

	if (!info.arguments.empty())
	{
		std::cout << "Arguments (" << info.arguments.size() << "):\n";
		for (const auto& arg : info.arguments)
		{
			std::cout
				<< "  - Name      : " << arg.name << "\n"
				<< "    Index     : " << arg.index << "\n"
				<< "    Type      : " << arg.type << "\n"
				<< "    Data Type : " << arg.dataType << "\n"
				<< "    Data Size : " << arg.dataSize << " bytes\n\n";
		}
	}

	if (info.threadgroupMemory.has_value())
	{
		std::cout << "Threadgroup Memory:\n";
		for (const auto& tg : info.threadgroupMemory.value())
		{
			std::cout
				<< "  - Index     : " << tg.index << "\n"
				<< "    Size      : " << tg.size << " bytes\n"
				<< "    Alignment : " << tg.alignment << " bytes\n\n";
		}
		std::cout << "\n";
	}

	std::cout << "=====================================================\n\n";
}

int main()
{
	lune::setWorkingDirectory();
	auto& context = lune::metal::MetalContext::instance();

	// CPU input data
	std::vector<float> a(arrayLength), b(arrayLength);
	for (size_t i = 0; i < arrayLength; ++i)
	{
		a[i] = static_cast<float>(rand()) / RAND_MAX;
		b[i] = static_cast<float>(rand()) / RAND_MAX;
	}

	// GPU buffers
	auto outputAdd = NS::TransferPtr(context.device()->newBuffer(arrayLength * sizeof(float),
	                                                             MTL::ResourceStorageModeShared));
	auto outputMul = NS::TransferPtr(context.device()->newBuffer(arrayLength * sizeof(float),
	                                                             MTL::ResourceStorageModeShared));

	// Create shader
	lune::metal::ComputeShaderCreateInfo info{
		.path = "shaders/basic.metal",
		.kernels = {"add_arrays", "multiply_arrays"}
	};

	// Add our buffers to our shader
	auto shader = lune::metal::ComputeShader(info)
	              .setBuffer("inputA", a)
	              .setBuffer("inputB", b)
	              .setBuffer("outputAdd", outputAdd)
	              .setBuffer("outputMul", outputMul);

	printKernelInfo(shader.kernelReflection("add_arrays"));

	// Dispatch GPU
	lune::Timer timer;
	timer.start();
	for (int i = 0; i < iterations; ++i)
	{
		printProgress(i, iterations);
		shader.dispatch("add_arrays", arrayLength)
		      .dispatch("multiply_arrays", arrayLength);
	}
	printProgress(iterations, iterations);
	auto duration = timer.delta() * 1000;
	std::cout << "GPU add/multiply time: " << duration << " ms\n";

	// Copy results back
	std::vector<float> gpuAdd(arrayLength), gpuMul(arrayLength);
	std::memcpy(gpuAdd.data(), outputAdd->contents(), arrayLength * sizeof(float));
	std::memcpy(gpuMul.data(), outputMul->contents(), arrayLength * sizeof(float));

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
		if (std::abs(gpuAdd[i] - addOut[i]) > 1e-5f || std::abs(gpuMul[i] - mulOut[i]) > 1e-5f)
		{
			correct = false;
			std::cerr << "Mismatch at index " << i << ": GPU(" << gpuAdd[i] << "," << gpuMul[i]
				<< ") vs CPU(" << addOut[i] << "," << mulOut[i] << ")\n";
			break;
		}
	}
	if (correct)
		std::cout << "GPU results verified!\n";

	return 0;
}
