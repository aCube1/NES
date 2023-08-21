// "Portable" BitFields implementation
// https://blog.codef00.com/2014/12/06/portable-bitfields-using-c11
//
// This implementations is UB (Undefined Behaviour).
// Some changes are made.

#ifndef _COMMON_BITFIELD_HPP_
#define _COMMON_BITFIELD_HPP_

#include "common/types.hpp"

#include <type_traits>

template <std::size_t last_bit>
struct MinimumTypeHelper {
	// clang-format off
	using type =
		typename std::conditional_t<last_bit == 0, void,
		typename std::conditional_t<last_bit <= 8, u8,
		typename std::conditional_t<last_bit <= 16, u16,
		typename std::conditional_t<last_bit <= 32, u32,
		typename std::conditional_t<last_bit <= 64, u64,
		void>>>>>;
	// clang-format on
};

template <std::size_t index, std::size_t bits = 1>
class BitField {
private:
	enum {
		MASK = (1U << bits) - 1U,
	};

	using T = typename MinimumTypeHelper<index + bits>::type;

public:
	BitField() = default;
	BitField(const BitField& other) = default;

	BitField& operator=(const BitField& other) {
		*this = T(other);
		return *this;
	}

	BitField& operator=(T value) {
		m_data = (value & ~(MASK << index)) | ((value & MASK) << index);
		return *this;
	}

	operator T() const { return (m_data >> index) & MASK; }

	explicit operator bool() { return m_data & (MASK << index); }

	BitField& operator++() { return *this = *this + 1; }

	BitField& operator--() { return *this = *this - 1; }

	T operator++(int) {
		T tmp = *this;
		++(*this);
		return tmp;
	}

	T operator--(int) {
		T tmp = *this;
		--(*this);
		return tmp;
	}

private:
	T m_data;
};

template <std::size_t index>
class BitField<index, 1> {
private:
	enum {
		BITS = 1,
		MASK = 0x01,
	};

	using T = typename MinimumTypeHelper<index + BITS>::type;

public:
	BitField() = default;
	BitField(const BitField& other) = default;

	BitField& operator=(const BitField& other) {
		*this = bool(other);
		return *this;
	}

	BitField& operator=(bool value) {
		m_data = (m_data & ~(MASK << index)) | (value << index);
		return *this;
	}

	operator bool() const { return m_data & (MASK << index); }

private:
	T m_data;
};

#endif // _COMMON_BITFIELD_HPP_
