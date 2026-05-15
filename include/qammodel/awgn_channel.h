#pragma once

#include <cstdint>
#include <random>

#include "base.h"

class awgn_channel {
	std::mt19937 m_rng;

public:
	explicit awgn_channel(std::uint32_t seed = std::random_device{}());

	void add_noise(std::vector<symbol_t> &symbols, double noise_variance);
};