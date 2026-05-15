

#include <stdexcept>

#include <qammodel/random_bit_sequence.h>

random_bit_sequence_generator::random_bit_sequence_generator(seed_type seed) : m_rng(seed) {}

bit_sequence random_bit_sequence_generator::generate(std::size_t bit_count) {
	bit_sequence bits;
	bits.reserve(bit_count);

	for(std::size_t i = 0; i < bit_count; ++i) {
		bits.push_back(m_bit_distribution(m_rng));
	}

	return bits;
}

bit_sequence random_bit_sequence_generator::generate_with_probability(
	std::size_t bit_count, double probability_of_one) {
	if(probability_of_one < 0.0 || probability_of_one > 1.0) {
		throw std::invalid_argument("Probability of one must be in range [0, 1]");
	}

	std::bernoulli_distribution distribution(probability_of_one);

	bit_sequence bits;
	bits.reserve(bit_count);

	for(std::size_t i = 0; i < bit_count; ++i) {
		bits.push_back(distribution(m_rng));
	}

	return bits;
}

void random_bit_sequence_generator::reseed(seed_type seed) { m_rng.seed(seed); }