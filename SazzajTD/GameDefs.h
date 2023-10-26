#ifndef GAME_DEFS_H
#define GAME_DEFS_H

struct tGamePosition
{
	float x, y;

	tGamePosition() : x( 0.f ), y( 0.f ) {}
	tGamePosition( float v ): x( v ), y( v ) {}
	tGamePosition( float _x, float _y ): x( _x ), y( _y ) {}
};

struct tGameTransform
{
	tGamePosition	position;
	float			rotation	= 0.f;
	float			scale		= 1.f;
};

#endif