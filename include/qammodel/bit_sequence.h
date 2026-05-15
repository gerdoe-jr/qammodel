#pragma once

#include <cstdint>
#include <limits>
#include <span>
#include <stdexcept>
#include <vector>

class bit_sequence {
public:
	using block_type = std::size_t;
	static constexpr std::size_t s_bits_per_block = std::numeric_limits<block_type>::digits;

private:
	std::size_t m_seq_length = 0;
	std::vector<block_type> m_blocks;

public:
	bit_sequence() = default;

	explicit bit_sequence(std::size_t bit_count);

	[[nodiscard]]
	std::size_t size() const noexcept;

	[[nodiscard]]
	bool empty() const noexcept;

	[[nodiscard]]
	std::size_t block_count() const noexcept;

	[[nodiscard]]
	std::span<const block_type> blocks() const noexcept;

	[[nodiscard]]
	bool get(std::size_t bit_index) const;

	void set(std::size_t bit_index, bool value);

	void push_back(bool value);

	void reserve(std::size_t bit_capacity);

	void resize(std::size_t new_bit_count, bool value = false);

	void clear() noexcept;

	[[nodiscard]]
	std::uint32_t read_as_uint(std::size_t offset, std::size_t count) const;

	void append_from_uint(std::uint32_t value, std::size_t count);

	[[nodiscard]]
	std::size_t count_errors_against(const bit_sequence &other) const;

private:
	struct bit_location {
		std::size_t block_index;
		std::size_t offset;
	};

	[[nodiscard]]
	static constexpr std::size_t blocks_required(std::size_t bit_count) noexcept {
		return (bit_count + s_bits_per_block - 1u) / s_bits_per_block;
	}

	[[nodiscard]]
	static constexpr bit_location locate(std::size_t bit_index) noexcept {
		return {.block_index = bit_index / s_bits_per_block, .offset = bit_index % s_bits_per_block};
	}

	inline void check_bit_index(std::size_t bit_index) const {
		if(bit_index >= m_seq_length) {
			throw std::out_of_range("bit_sequence index is out of range");
		}
	}

	void clear_unused_tail_bits() noexcept;
};

[[nodiscard]]
double bit_error_rate(const bit_sequence &original, const bit_sequence &restored);