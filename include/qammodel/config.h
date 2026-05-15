#pragma once

#include <cstdint>
#include <numeric>

template<std::uint8_t k, typename value_type = std::uint16_t>
struct qam_config {
	static_assert(2 * k < std::numeric_limits<value_type>::digits, "QAM order does not fit into value_type");

	static constexpr value_type s_order = value_type{1} << (2 * k); // 2^(2 * k)
	static constexpr value_type s_bits_per_symbol = 2 * k; // log2(order)
	static constexpr value_type s_levels_per_axis = value_type{1} << k; // sqrt(order)
	static constexpr value_type s_bits_per_axis = k; // bits_per_symbol / 2

	static constexpr double s_avg_symbol_energy = (2.0 / 3.0) * static_cast<double>(value_type{1} << (2 * k) - 1); // 2/3 * (order - 1)
};
