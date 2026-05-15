#pragma once

#include <cstddef>
#include <cstdint>
#include <random>

#include "bit_sequence.h"

class random_bit_sequence_generator {
	std::mt19937 m_rng;
	std::bernoulli_distribution m_bit_distribution{0.5};

public:
	using seed_type = std::uint32_t;

	explicit random_bit_sequence_generator(seed_type seed = std::random_device{}());

	[[nodiscard]]
	bit_sequence generate(std::size_t bit_count);

	[[nodiscard]]
	bit_sequence generate_with_probability(std::size_t bit_count, double probability_of_one);

	void reseed(seed_type seed);

private:
	[[nodiscard]]
	static constexpr std::size_t blocks_required(std::size_t bit_count) noexcept {
		return (bit_count + bit_sequence::s_bits_per_block - 1U) / bit_sequence::s_bits_per_block;
	}
};