#ifndef ANIMATED_TEXTURE_H
#define ANIMATED_TEXTURE_H

#include <memory>
#include <string>
#include "Utilities.h"

struct SDL_Texture;
void DestroyTexture(SDL_Texture*);
using tTextureSafePtr = std::unique_ptr<SDL_Texture, decltype(&DestroyTexture)>;

class cAnimatedTexture
{
public:
	cAnimatedTexture();
	~cAnimatedTexture();

	[[nodiscard]]
	static	std::unique_ptr<cAnimatedTexture>	Load( const std::string& path );

			void								SetDimensions( int rows, int cols );
			void								SetFramerate( float framerate );
			void								SetPosition( const tVector2Df& position );
			void								SetPriority(int priority);
			void								SetScale(float scale);

			void								GetFrameDims( float& frameWidth, float& frameHeight ) const;

	virtual void								Update( float deltaTime );
	virtual void								Draw();

protected:
			tVector2Df							m_position;
			float								m_scale				= 1.f;
			int									m_rows				= 1;
			int									m_cols				= 1;
			float								m_framerate			= 1.f;
			float								m_framerateDelta	= 0.016f;
			int									m_frame				= 0;
			float								m_frameWidth		= 0.f;
			float								m_frameHeight		= 0.f;
			int									m_priority			= 1;

			float								m_currFrameTime		= 0.f;

			tTextureSafePtr						m_texture{ nullptr, &DestroyTexture };
	
};

#endif