#pragma once
#include <cinttypes>
#include "vector.hxx"

#define NUMZERO_COLOR_CUSTOM_FLOAT template <typename Float, typename = typename std::enable_if<std::is_floating_point<Float>::value>::type>

struct Color
{
	union
	{
		std::uint32_t value;
		std::uint8_t data[4];
		struct
		{
			std::uint8_t r, g, b, a;
		};
		struct
		{
			std::uint8_t red, green, blue;
			union
			{
				std::uint8_t alpha;
				std::uint8_t opacity;
			};
		};
	};

	NUMZERO_COLOR_CUSTOM_FLOAT
	static std::uint8_t convert(Float x)
	{
		int value(255.0 * x);
		if(value <= 0)
			return 0;
		if(value >= 255)
			return 255;
		return value;
	}

	Color() = default;

	Color(std::uint32_t value) :
		value(value)
	{
	}

	Color(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a = 255) :
		r(r),
		g(g),
		b(b),
		a(a)
	{
	}

	NUMZERO_COLOR_CUSTOM_FLOAT
	Color(Float r, Float g, Float b, Float a = 1.0)
	{
		setR(r);
		setG(g);
		setB(b);
		setA(a);
	}

	NUMZERO_COLOR_CUSTOM_FLOAT
	Color(Float value)
	{
		setR(value);
		setG(value);
		setB(value);
		setA(1.0);
	}

	NUMZERO_COLOR_CUSTOM_FLOAT
	Color(Vector<Float, 3> color)
	{
		setR(color[0]);
		setG(color[1]);
		setB(color[2]);
		setA(1.0);
	}

	NUMZERO_COLOR_CUSTOM_FLOAT
	Color(Vector<Float, 4> color)
	{
		setR(color[0]);
		setG(color[1]);
		setB(color[2]);
		setA(color[3]);
	}

	NUMZERO_COLOR_CUSTOM_FLOAT	Float getR() const { return r / 255.0; }
	NUMZERO_COLOR_CUSTOM_FLOAT	Float getG() const { return g / 255.0; }
	NUMZERO_COLOR_CUSTOM_FLOAT	Float getB() const { return b / 255.0; }
	NUMZERO_COLOR_CUSTOM_FLOAT	Float getA() const { return a / 255.0; }

	NUMZERO_COLOR_CUSTOM_FLOAT	void setR(Float x) { r = convert(x); }
	NUMZERO_COLOR_CUSTOM_FLOAT	void setG(Float x) { g = convert(x); }
	NUMZERO_COLOR_CUSTOM_FLOAT	void setB(Float x) { b = convert(x); }
	NUMZERO_COLOR_CUSTOM_FLOAT	void setA(Float x) { a = convert(x); }
};

#undef NUMZERO_COLOR_CUSTOM_FLOAT
