
#include <bit>

#include <qammodel/bit_sequence.h>

bit_sequence::bit_sequence(std::size_t bit_count) :
	m_seq_length(bit_count), m_blocks(blocks_required(bit_count), block_type{0}) {}

std::size_t bit_sequence::size() const noexcept { return m_seq_length; }

bool bit_sequence::empty() const noexcept { return m_seq_length == 0; }

std::size_t bit_sequence::block_count() const noexcept { return m_blocks.size(); }

std::span<const bit_sequence::block_type> bit_sequence::blocks() const noexcept { return m_blocks; }

bool bit_sequence::get(std::size_t bit_index) const {
	check_bit_index(bit_index);

	const auto [block_index, offset] = locate(bit_index);

	return (m_blocks[block_index] >> offset) & block_type{1};
}

void bit_sequence::set(std::size_t bit_index, bool value) {
	check_bit_index(bit_index);

	const auto [block_index, offset] = locate(bit_index);
	const block_type mask = block_type{1} << offset;

	if(value) {
		m_blocks[block_index] |= mask;
	} else {
		m_blocks[block_index] &= ~mask;
	}
}

void bit_sequence::push_back(bool value) {
	if(m_seq_length % s_bits_per_block == 0) {
		m_blocks.push_back(block_type{0});
	}

	const std::size_t index = m_seq_length;
	++m_seq_length;

	set(index, value);
}

void bit_sequence::reserve(std::size_t bit_capacity) { m_blocks.reserve(blocks_required(bit_capacity)); }

void bit_sequence::resize(std::size_t new_bit_count, bool value) {
	const std::size_t old_bit_count = m_seq_length;

	m_seq_length = new_bit_count;
	m_blocks.resize(blocks_required(new_bit_count), block_type{0});

	if(value && new_bit_count > old_bit_count) {
		for(std::size_t i = old_bit_count; i < new_bit_count; ++i) {
			set(i, true);
		}
	}

	clear_unused_tail_bits();
}

void bit_sequence::clear() noexcept {
	m_seq_length = 0;
	m_blocks.clear();
}

std::uint32_t bit_sequence::read_as_uint(std::size_t offset, std::size_t count) const {
	if(count > 32) {
		throw std::invalid_argument("Cannot read more than 32 bits into uint32_t");
	}

	if(offset > m_seq_length || count > m_seq_length - offset) {
		throw std::out_of_range("BitSequence read is out of range");
	}

	if(count == 0) {
		return 0;
	}

	const std::size_t block_index = offset / s_bits_per_block;
	const std::size_t bit_offset = offset % s_bits_per_block;

	block_type value = m_blocks[block_index] >> bit_offset;

	const std::size_t available_in_first_block = s_bits_per_block - bit_offset;

	if(count > available_in_first_block) {
		value |= m_blocks[block_index + 1] << available_in_first_block;
	}

	const block_type mask = (block_type{1} << count) - block_type{1};

	return static_cast<std::uint32_t>(value & mask);
}

void bit_sequence::append_from_uint(std::uint32_t value, std::size_t count) {
	if(count > 32) {
		throw std::invalid_argument("Cannot append more than 32 bits from uint32_t");
	}

	reserve(m_seq_length + count);

	for(std::size_t i = 0; i < count; ++i) {
		push_back(((value >> i) & 1U) != 0);
	}
}

std::size_t bit_sequence::count_errors_against(const bit_sequence &other) const {
	if(m_seq_length != other.m_seq_length) {
		throw std::invalid_argument("Bit sequences must have the same size");
	}

	std::size_t errors = 0;

	const std::size_t full_blocks = m_seq_length / s_bits_per_block;
	const std::size_t tail_bits = m_seq_length % s_bits_per_block;

	for(std::size_t i = 0; i < full_blocks; ++i) {
		errors += static_cast<std::size_t>(std::popcount(m_blocks[i] ^ other.m_blocks[i]));
	}

	if(tail_bits != 0) {
		const block_type mask = (block_type{1} << tail_bits) - block_type{1};

		errors += static_cast<std::size_t>(
			std::popcount((m_blocks[full_blocks] ^ other.m_blocks[full_blocks]) & mask));
	}

	return errors;
}

void bit_sequence::clear_unused_tail_bits() noexcept {
	if(m_blocks.empty()) {
		return;
	}

	const std::size_t used_tail_bits = m_seq_length % s_bits_per_block;

	if(used_tail_bits == 0) {
		return;
	}

	const block_type mask = (block_type{1} << used_tail_bits) - block_type{1};
	m_blocks.back() &= mask;
}

double bit_error_rate(const bit_sequence &original, const bit_sequence &restored) {
	if(original.empty()) {
		return 0.0;
	}

	const std::size_t errors = original.count_errors_against(restored);

	return static_cast<double>(errors) / static_cast<double>(original.size());
}