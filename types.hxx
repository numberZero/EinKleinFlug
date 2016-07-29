#pragma once
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-attributes"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#pragma GCC diagnostic ignored "-Wmisleading-indentation"
#include <Eigen/Core>
#pragma GCC diagnostic pop

typedef double Float;
typedef Eigen::Vector2d Vector2;

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

	Color(Float r, Float g, Float b, Float a = 1.0)
	{
		setR(r);
		setG(g);
		setB(b);
		setA(a);
	}

	Float getR() const { return r / 255.0; }
	Float getG() const { return g / 255.0; }
	Float getB() const { return b / 255.0; }
	Float getA() const { return a / 255.0; }

	void setR(Float x) { r = convert(x); }
	void setG(Float x) { g = convert(x); }
	void setB(Float x) { b = convert(x); }
	void setA(Float x) { a = convert(x); }
};
