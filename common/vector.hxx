#pragma once
#include <cmath>
#include <stdexcept>
#include <type_traits>

template <typename Scalar, int Length>
struct Vector
{
	static_assert(Length >= 1, "Invalid vector length");
	static_assert(std::is_floating_point<Scalar>::value, "Vector must have floating-point elements");
	constexpr static int const n = Length;
	constexpr static int const N = Length;

	Vector() = default;
	Vector(Vector const &) = default;
	Vector &operator= (Vector const &) = default;

	void setZero()
	{
		std::fill(data, data + N, Scalar(0));
	}

	Scalar dot(Vector const &b) const
	{
		return *this * b;
	}

	Scalar norm() const
	{
		return std::sqrt(squaredNorm());
	}

	Scalar squaredNorm() const
	{
		return *this * *this;
	}

	Scalar &operator[] (int index)
	{
		if(index < 0 || index >= N)
			throw std::out_of_range("Vector index out of range");
		return data[index];
	}

	Scalar const &operator[] (int index) const
	{
		if(index < 0 || index >= N)
			throw std::out_of_range("Vector index out of range");
		return data[index];
	}

	Vector &operator+= (Vector const &b)
	{
		for(int i = 0; i != N; ++i)
			data[i] += b.data[i];
	}

	Vector &operator-= (Vector const &b)
	{
		for(int i = 0; i != N; ++i)
			data[i] -= b.data[i];
	}

	Vector &operator*= (Scalar c)
	{
		for(int i = 0; i != N; ++i)
			data[i] *= c;
	}

	Vector &operator/= (Scalar c)
	{
		*this *= 1 / c;
	}

	union
	{
		Scalar item[N];
		Scalar data[N];
	};
};

template <typename T, int n>
inline Vector<T, n> operator- (Vector<T, n> const &a)
{
	return T(-1) * a;
}

template <typename T, int n>
inline Vector<T, n> operator+ (Vector<T, n> const &a, Vector<T, n> const &b)
{
	return Vector<T, n>(a) += b;
}

template <typename T, int n>
inline Vector<T, n> operator- (Vector<T, n> const &a, Vector<T, n> const &b)
{
	return Vector<T, n>(a) -= b;
}

template <typename T, int n>
inline Vector<T, n> operator* (Vector<T, n> const &a, T c)
{
	return Vector<T, n>(a) *= c;
}

template <typename T, int n>
inline Vector<T, n> operator* (T c, Vector<T, n> const &a)
{
	return Vector<T, n>(a) *= c;
}

template <typename T, int n>
inline Vector<T, n> operator/ (Vector<T, n> const &a, T c)
{
	return Vector<T, n>(a) /= c;
}

template <typename T, int n>
inline T operator* (Vector<T, n> const &a, Vector<T, n> const &b)
{
	T x = 0;
	for(int i = 0; i != n; ++i)
		x += a[i] * b[i];
	return x;
}

template <typename T>
inline Vector<T, 2> directionVector(T angle, T length = T(1))
{
	return {
		length * std::cos(angle),
		length * std::sin(angle)
	};
}
