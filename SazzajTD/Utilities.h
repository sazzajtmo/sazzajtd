#ifndef UTILITIES_H
#define UTILITIES_H

#include <cmath>
#include <cfloat>

struct tColor
{
	float r = 1.f;
	float g = 1.f;
	float b = 1.f;
	float a = 1.f;

	tColor(){}
	tColor( float _r, float _g, float _b ) : r(_r), g(_g), b(_b), a( 1.f ) {}
	tColor( float _r, float _g, float _b, float _a ) : r(_r), g(_g), b(_b), a(_a) {}
	
	/// <summary>
	/// 0xAARRGGBB
	/// </summary>
	/// <param name="hex"></param>
	tColor( unsigned int hex )
	{		
		a = static_cast<float>( ( hex >> 24 ) & 0xff ) / 255.f;
		r = static_cast<float>( ( hex >> 16 ) & 0xff ) / 255.f;
		g = static_cast<float>( ( hex >>  8 ) & 0xff ) / 255.f;
		b = static_cast<float>( ( hex >>  0 ) & 0xff ) / 255.f;
	}

};

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

static const tVector2Df X_AXIS{ 1.f, 0.f };
static const tVector2Df Y_AXIS{ 0.f, -1.f };	//+1 is up, -1 is down

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


template<typename T = float>
struct tRect
{
	T x{}, y{}, w{}, h{};

	tRect() : x(), y(), w(), h() {}
	tRect( T v ): x( v ), y( v ), w( v ), h( v ) {}
	tRect( T _x, T _y, T _w, T _h ): x( _x ), y( _y ), w( _w ), h( _h ) {}
};

using tRectf = tRect<float>;

#endif