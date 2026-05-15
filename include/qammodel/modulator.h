#pragma once

#include <cstdint>
#include <vector>

#include "base.h"
#include "bit_sequence.h"
#include "config.h"

// header only implementation since this class is a template
// extern template class in case you need to separate this file in .h/.cpp binding

template<std::uint8_t modulation_level>
class qam_modulator {
	using config_t = qam_config<modulation_level>;

public:
	qam_modulator() = default;

	std::vector<symbol_t> modulate(const bit_sequence &input_data) const {
		if(input_data.size() % config_t::s_bits_per_symbol != 0) {
			throw std::invalid_argument("The number of bits must be divisible by s_bits_per_symbol");
		}

		std::vector<symbol_t> symbols;
		symbols.reserve(input_data.size() / config_t::s_bits_per_symbol);

		const double norm = std::sqrt(config_t::s_avg_symbol_energy);

		for(std::size_t offset = 0; offset < input_data.size(); offset += config_t::s_bits_per_symbol) {
			const std::uint32_t i_bits = input_data.read_as_uint(offset, config_t::s_bits_per_axis);
			const std::uint32_t q_bits =
				input_data.read_as_uint(offset + config_t::s_bits_per_axis, config_t::s_bits_per_axis);

			const double i = axis_level_from_gray_index(i_bits);
			const double q = axis_level_from_gray_index(q_bits);

			symbols.emplace_back(i / norm, q / norm);
		}

		return symbols;
	}

private:
	[[nodiscard]]
	static double axis_level_from_gray_index(std::uint32_t gray_index) {
		const std::uint32_t binary_index = gray_to_binary(gray_index);

		return 2.0 * static_cast<double>(binary_index) - static_cast<double>(config_t::s_levels_per_axis - 1);
	}
};
