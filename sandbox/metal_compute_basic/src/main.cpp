#include <Metal/Metal.hpp>
#include <iostream>
#include <cmath>
#include <vector>
import lune;

constexpr size_t arrayLength = 1 << 26;


class CustomShader final : public lune::metal::ComputeShader
{
	NS::SharedPtr<MTL::Buffer> m_inputA;
	NS::SharedPtr<MTL::Buffer> m_inputB;
	NS::SharedPtr<MTL::Buffer> m_outputAdd;
	NS::SharedPtr<MTL::Buffer> m_outputMul;
	NS::UInteger m_bufferSize;

public:
	explicit CustomShader(const lune::metal::ComputeShaderCreateInfo& info) :
		ComputeShader(info),
		m_bufferSize(arrayLength * sizeof(float))
	{
		createBuffers();
	}

	void encodeComputeCommand(MTL::ComputeCommandEncoder* encoder) override
	{
		for (size_t i = 0; i < m_pipelines.size(); ++i)
		{
			encoder->setComputePipelineState(m_pipelines[i].get());
			encoder->setBuffer(m_inputA.get(), 0, 0);
			encoder->setBuffer(m_inputB.get(), 0, 1);

			if (i == 0)
				encoder->setBuffer(m_outputAdd.get(), 0, 2);
			else
				encoder->setBuffer(m_outputMul.get(), 0, 2);

			MTL::Size gridSize = MTL::Size(arrayLength, 1, 1);
			NS::UInteger threadGroupSize = m_pipelines[i]->maxTotalThreadsPerThreadgroup();
			if (threadGroupSize > arrayLength)
				threadGroupSize = arrayLength;
			const MTL::Size threadsPerGroup = MTL::Size(threadGroupSize, 1, 1);

			encoder->dispatchThreads(gridSize, threadsPerGroup);
		}
	}

	void createBuffers()
	{
		m_inputA = NS::TransferPtr(
			m_device->newBuffer(m_bufferSize, MTL::ResourceStorageModeShared));
		m_inputB = NS::TransferPtr(
			m_device->newBuffer(m_bufferSize, MTL::ResourceStorageModeShared));
		m_outputAdd = NS::TransferPtr(
			m_device->newBuffer(m_bufferSize, MTL::ResourceStorageModeShared));
		m_outputMul = NS::TransferPtr(
			m_device->newBuffer(m_bufferSize, MTL::ResourceStorageModeShared));

		fillRandom(m_inputA);
		fillRandom(m_inputB);
	}

	void fillRandom(const NS::SharedPtr<MTL::Buffer>& buf)
	{
		float* ptr = static_cast<float*>(buf->contents());
		for (unsigned long i = 0; i < arrayLength; ++i)
			ptr[i] = static_cast<float>(rand()) / RAND_MAX;
	}

	void verifyResults()
	{
		float* a = static_cast<float*>(m_inputA->contents());
		float* b = static_cast<float*>(m_inputB->contents());
		float* addR = static_cast<float*>(m_outputAdd->contents());
		float* mulR = static_cast<float*>(m_outputMul->contents());

		bool ok = true;
		for (unsigned long i = 0; i < arrayLength; ++i)
		{
			if (std::fabs(addR[i] - (a[i] + b[i])) > 1e-6f)
			{
				std::cout << "Add ERROR at index " << i << "\n";
				ok = false;
				break;
			}
			if (std::fabs(mulR[i] - (a[i] * b[i])) > 1e-6f)
			{
				std::cout << "Mul ERROR at index " << i << "\n";
				ok = false;
				break;
			}
		}
		if (ok)
			std::cout << "Compute results verified for both kernels!\n";
	}
};

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


int main()
{
	lune::setWorkingDirectory();
	auto& context = lune::metal::MetalContext::instance();

	// Create our compute kernels/shader
	lune::metal::ComputeShaderCreateInfo info{
		.path = "shaders/basic.metal",
		.kernels = {"add_arrays", "multiply_arrays"}
	};
	auto shader = std::make_shared<CustomShader>(info);
	context.addShader(shader);

	// Calculate the time for the gpu version
	lune::Timer timer;
	timer.start();
	context.compute();
	auto duration = timer.delta() * 1000;

	double ms = std::chrono::duration<double, std::milli>(duration).count();
	std::cout << "GPU add/multiply time: " << ms << " ms\n";
	shader->verifyResults(); // Verify our results are actually correct
	shader.reset();
	// Run same calculation on the CPU to compare speeds
	std::vector<float> a(arrayLength), b(arrayLength), addOut(arrayLength), mulOut(arrayLength);

	// Fill random data
	for (size_t i = 0; i < arrayLength; ++i)
	{
		a[i] = static_cast<float>(rand()) / RAND_MAX;
		b[i] = static_cast<float>(rand()) / RAND_MAX;
	}

	// Record CPU execution time
	timer.start();
	cpuAddMultiply(a, b, addOut, mulOut);
	duration = timer.delta() * 1000;

	ms = std::chrono::duration<double, std::milli>(duration).count();
	std::cout << "CPU add/multiply time: " << ms << " ms\n";


	return 0;
}
