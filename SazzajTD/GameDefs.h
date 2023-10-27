#ifndef GAME_DEFS_H
#define GAME_DEFS_H

#include <cmath>
#include <cfloat>

template<typename T = float>
struct tVector2D
{
	T x{}, y{};

	tVector2D() : x(), y() {}
	tVector2D( T v ): x( v ), y( v ) {}
	tVector2D( T _x, T _y ): x( _x ), y( _y ) {}

	tVector2D<T>& operator+=(const tVector2D<T>& rh)
	{
		x += rh.x;
		y += rh.y;
		return *this;
	}
};

using tVector2Df = tVector2D<float>;

struct tGameTransform
{
	tVector2Df			position;
	float				rotation	= 0.f;
	float				scale		= 1.f;
};

template<typename T>
T distance(const tVector2D<T>& lh, const tVector2D<T>& rh)
{
	return static_cast<T>( std::sqrt( ( lh.x - rh.x ) * ( lh.x - rh.x ) + ( lh.y - rh.y ) * ( lh.y - rh.y ) ) );
}

template<typename T>
T magnitude(const tVector2D<T>& vec)
{
	return static_cast<T>( std::sqrt( vec.x * vec.x + vec.y * vec.y ) );
}

template<typename T>
tVector2D<T> normalize(const tVector2D<T>& vec)
{
	T mag = magnitude( vec );

	if( mag < FLT_EPSILON )
		return vec;
	
	return tVector2D<T>( vec.x / mag, vec.y / mag );
}

//ab = b - a;
template<typename T>
tVector2D<T> direction(const tVector2D<T>& a, const tVector2D<T>& b)
{
	return b-a;
}

template<typename T>
tVector2D<T> directionNormalized(const tVector2D<T>& a, const tVector2D<T>& b)
{
	return normalize(b-a);
}

template<typename T>
tVector2D<T> operator+(const tVector2D<T>& lh, const tVector2D<T>& rh)
{
	return tVector2D<T>( lh.x + rh.x, lh.y + rh.y );
}

template<typename T>
tVector2D<T> operator-(const tVector2D<T>& lh, const tVector2D<T>& rh)
{
	return tVector2D<T>( lh.x - rh.x, lh.y - rh.y );
}

template<typename T>
tVector2D<T> operator*(const tVector2D<T>& lh, float value)
{
	return tVector2D<T>( lh.x * value, lh.y * value );
}

#endif