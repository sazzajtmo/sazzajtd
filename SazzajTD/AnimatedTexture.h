#ifndef ANIMATED_TEXTURE_H
#define ANIMATED_TEXTURE_H

#include <memory>
#include <string>
#include "GameDefs.h"

struct SDL_Texture;


class cAnimatedTexture
{
	cAnimatedTexture();
	~cAnimatedTexture();

public:
	static	cAnimatedTexture*				Load( const std::string& path );
	static	void							Destroy( cAnimatedTexture* animTexture );
			cAnimatedTexture*				SetDimensions( int rows, int cols );
			cAnimatedTexture*				SetFramerate( float framerate );
			cAnimatedTexture*				SetPosition( const tVector2Df& position );
	virtual void							Update( float deltaTime );
	virtual void							Draw();

protected:
			tVector2Df						m_position;
			int								m_rows				= 1;
			int								m_cols				= 1;
			float							m_framerate			= 1.f;
			float							m_framerateDelta	= 0.016f;
			int								m_frame				= 0;
			float							m_frameWidth		= 0.f;
			float							m_frameHeight		= 0.f;

			float							m_currFrameTime		= 0.f;

			SDL_Texture*					m_texture			= nullptr;
	
};

#endif