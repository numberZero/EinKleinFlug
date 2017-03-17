#pragma once
#include <cmath>
#include <initializer_list>
#include <stdexcept>
#include <type_traits>
#include "vector.hxx"

template <typename Scalar, int Size>
struct Matrix
{
	static_assert(Size >= 1, "Invalid matrix size");
	static_assert(std::is_floating_point<Scalar>::value, "Matrix must have floating-point elements");
	constexpr static int const n = Size;
	constexpr static int const N = Size * Size;

	Matrix() = default;
	Matrix(Matrix const &) = default;
	Matrix &operator= (Matrix const &) = default;

	Matrix &operator= (std::initializer_list<Scalar> list)
	{
		if(list.size() != N)
			throw std::logic_error("Matrix initialization list size is invalid");
		auto iter = list.begin(); // not std::copy as it would be transposed
		for(int i = 0; i != n; ++i)
			for(int j = 0; j != n; ++j)
			{
				item[j][i] = *iter;
				++iter;
			}
		return *this;
	}

	void setZero()
	{
		std::fill(data, data + N, Scalar(0));
	}

	void setIdentity()
	{
		for(int j = 0; j != n; ++j)
			for(int i = 0; i != n; ++i)
				item[j][i] = i == j ? 1 : 0;
	}

	Scalar &operator() (int i, int j)
	{
		if(i < 0 || i >= n || j < 0 || j >= n)
			throw std::out_of_range("Matrix index out of range");
		return item[j][i];
	}

	Scalar const &operator() (int i, int j) const
	{
		if(i < 0 || i >= n || j < 0 || j >= n)
			throw std::out_of_range("Matrix index out of range");
		return item[j][i];
	}

	Matrix &operator+= (Matrix const &b)
	{
		for(int k = 0; k != N; ++k)
			data[k] += b.data[k];
		return *this;
	}

	Matrix &operator-= (Matrix const &b)
	{
		for(int k = 0; k != N; ++k)
			data[k] -= b.data[k];
		return *this;
	}

	Matrix &operator*= (Matrix const &b)
	{
		return *this = *this * b;
	}

	Matrix &operator*= (Scalar c)
	{
		for(int k = 0; k != N; ++k)
			data[k] *= c;
		return *this;
	}

	Matrix &operator/= (Scalar c)
	{
		return *this *= 1 / c;
	}

	union
	{
		Scalar item[Size][Size];
		Scalar data[Size * Size];
	};
};

template <typename T, int n>
inline Matrix<T, n> operator+ (Matrix<T, n> const &a, Matrix<T, n> const &b)
{
	return Matrix<T, n>(a) += b;
}

template <typename T, int n>
inline Matrix<T, n> operator- (Matrix<T, n> const &a, Matrix<T, n> const &b)
{
	return Matrix<T, n>(a) -= b;
}

template <typename T, int n>
inline Matrix<T, n> operator* (Matrix<T, n> const &a, T c)
{
	return Matrix<T, n>(a) *= c;
}

template <typename T, int n>
inline Matrix<T, n> operator* (T c, Matrix<T, n> const &a)
{
	return Matrix<T, n>(a) *= c;
}

template <typename T, int n>
inline Matrix<T, n> operator/ (Matrix<T, n> const &a, T c)
{
	return Matrix<T, n>(a) /= c;
}

template <typename T, int n>
inline Matrix<T, n> operator* (Matrix<T, n> const &a, Matrix<T, n> const &b)
{
	Matrix<T, n> x;
	for(int j = 0; j != n; ++j)
		for(int i = 0; i != n; ++i)
		{
			T s = 0;
			for(int k = 0; k != n; ++k)
				s += a(i, k) * b(k, j);
			x(i, j) = s;
		}
	return x;
}

template <typename T, int n>
inline Vector<T, n> operator* (Matrix<T, n> const &a, Vector<T, n> const &b)
{
	Vector<T, n> x;
	for(int i = 0; i != n; ++i)
	{
		T s = 0;
		for(int k = 0; k != n; ++k)
			s += a(i, k) * b[k];
		x[i] = s;
	}
	return x;
}
