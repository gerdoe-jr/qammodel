#pragma once

#include <algorithm>
#include <cstdint>
#include <vector>

#include "base.h"
#include "bit_sequence.h"
#include "config.h"

// header only implementation since this class is a template
// extern template class in case you need to separate this file in .h/.cpp binding

template<std::size_t modulation_level>
class qam_demodulator {
public:
	using config_t = qam_config<modulation_level>;

	[[nodiscard]]
	bit_sequence demodulate(const std::vector<symbol_t> &symbols) const {
		bit_sequence bits;
		bits.reserve(symbols.size() * config_t::s_bits_per_symbol);

		const double norm = std::sqrt(config_t::s_avg_symbol_energy);

		for(const symbol_t &symbol : symbols) {
			const std::uint32_t i_gray = nearest_gray_index(symbol.real() * norm);
			const std::uint32_t q_gray = nearest_gray_index(symbol.imag() * norm);

			bits.append_from_uint(i_gray, config_t::s_bits_per_axis);
			bits.append_from_uint(q_gray, config_t::s_bits_per_axis);
		}

		return bits;
	}

private:
	[[nodiscard]]
	static std::uint32_t nearest_gray_index(double value) {
		const double shifted = (value + static_cast<double>(config_t::s_levels_per_axis - 1)) / 2.0;
		const long rounded = std::lround(shifted);
		const long clipped = std::clamp(rounded, 0L, static_cast<long>(config_t::s_levels_per_axis - 1));

		return binary_to_gray(static_cast<std::uint32_t>(clipped));
	}
};