#pragma once
#include <cmath>
#include <cstdint>

#define PACKED __attribute__((packed))

template <unsigned precision>
struct PACKED Fixed_32
{
	std::int32_t value;

	Fixed_32() = default;

	Fixed_32(double b)
	{
		*this = b;
	}

	Fixed_32 &operator= (double b)
	{
		value = std::lround(std::ldexp(b, precision));
		return *this;
	}

	operator double()
	{
		return std::ldexp(value, -precision);
	}
};

typedef Fixed_32<16> fixed32_t;

struct PACKED FrameHeader
{
	std::uint32_t key;
	std::uint32_t frame_id;
	std::uint32_t ship_count;
};

struct PACKED FrameFooter
{
	std::uint32_t key;
	std::uint32_t frame_id;
};

struct PACKED ShipHeader
{
	std::uint8_t desc_present;
	std::uint8_t state_present;
};

struct PACKED ShipDesc
{
	fixed32_t recharge_rate;
	fixed32_t max_hp;
	fixed32_t armor;
};

struct PACKED ShipState
{
	fixed32_t position[3];
	fixed32_t velocity[3];
	fixed32_t hp;
};
