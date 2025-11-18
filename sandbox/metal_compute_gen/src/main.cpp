#include <iostream>
#include <vector>
#include <numeric>

import lune;

int main()
{
	lune::setWorkingDirectory();

	constexpr size_t N = 1 << 16; // smaller for histogram printing
	std::vector<float> cpuData(N, 0.0f);

	const lune::metal::ComputeShaderCreateInfo info = {
		.path = "shaders/basic.metal",
		.kernels = {"generate_random"},
	};

	auto shader = lune::metal::ComputeShader(info);
	auto kernel = shader.kernel("generate_random")
	                    .setBuffer("outBuffer", cpuData)
	                    .dispatch(N);

	const auto result = kernel.buffer("outBuffer");

	std::memcpy(cpuData.data(), result->contents(), sizeof(float) * N);

	printKernelInfo(kernel.reflection());

	// Compute mean/variance
	const double sum = std::accumulate(cpuData.begin(), cpuData.end(), 0.0);
	const double mean = sum / N;

	double sq_sum = 0.0;
	for (const auto v : cpuData)
		sq_sum += (v - mean) * (v - mean);
	const double variance = sq_sum / N;

	std::cout << "Generated " << N << " random numbers.\n";
	std::cout << "Mean: " << mean << ", Variance: " << variance << "\n\n";

	return 0;
}
