#include <qammodel/awgn_channel.h>

awgn_channel::awgn_channel(std::uint32_t seed) : m_rng(seed) {}

void awgn_channel::add_noise(std::vector<symbol_t> &symbols, double noise_variance) {
	if(noise_variance < 0.0) {
		throw std::invalid_argument("Noise variance must be non-negative");
	}

	const double sigma = std::sqrt(noise_variance / 2);
	std::normal_distribution<double> distribution(0.0, sigma);

	for(symbol_t &symbol : symbols) {
		symbol.real(symbol.real() + distribution(m_rng));
		symbol.imag(symbol.imag() + distribution(m_rng));
	}
}