#pragma once

#include <complex>
#include <cstdint>

using symbol_t = std::complex<double>;

constexpr std::uint32_t binary_to_gray(std::uint32_t value) { return value ^ (value >> 1u); }

constexpr std::uint32_t gray_to_binary(std::uint32_t gray) {
	std::uint32_t binary = 0;

	for(; gray != 0; gray >>= 1u) {
		binary ^= gray;
	}

	return binary;
}