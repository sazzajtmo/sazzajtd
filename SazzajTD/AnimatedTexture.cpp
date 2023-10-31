#include "AnimatedTexture.h"
#include <cmath>
#include "GameRenderer.h"
#include "SDL.h"
#include "SDL_surface.h"
#include "SDL_image.h"

cAnimatedTexture::cAnimatedTexture()
{
}

cAnimatedTexture::~cAnimatedTexture()
{
	if( m_texture )
		SDL_DestroyTexture( m_texture );
}

cAnimatedTexture* cAnimatedTexture::Load(const std::string& path)
{
	SDL_Texture* texture = cGameRenderer::GetInstance()->GetSDLTexture( path );

	if( !texture )
		return nullptr;

	cAnimatedTexture* animTexture = new cAnimatedTexture();

	animTexture->m_texture = texture;	
	animTexture->SetDimensions( 1, 1 );

	return animTexture;
}

void cAnimatedTexture::Destroy(cAnimatedTexture* animTexture)
{
	if( !animTexture )
		return;

	delete animTexture;
}

cAnimatedTexture* cAnimatedTexture::SetDimensions(int rows, int cols)
{
	m_rows = rows;
	m_cols = cols;

	int texWidth, texHeight;
	SDL_QueryTexture( m_texture, nullptr, nullptr, &texWidth, &texHeight );

	m_frameWidth	= static_cast<float>( texWidth / m_cols );
	m_frameHeight	= static_cast<float>( texHeight / m_rows );

	return this;
}

cAnimatedTexture* cAnimatedTexture::SetFramerate(float framerate)
{
	m_framerate			= framerate;
	m_framerateDelta	= 1.f / m_framerate;
	return this;
}

cAnimatedTexture* cAnimatedTexture::SetPosition(const tVector2Df& position)
{
	m_position = position;
	return this;
}

void cAnimatedTexture::Update(float deltaTime)
{
	m_currFrameTime += deltaTime;

	if (m_currFrameTime >= m_framerateDelta)
	{
		m_currFrameTime -= m_framerateDelta;
		m_frame = ( m_frame + 1 ) % ( m_rows * m_cols );
	}
}

void cAnimatedTexture::Draw()
{	
	int row = m_frame / m_cols;
	int col = m_frame % m_cols;

	tRectf posRect;
	posRect.x = m_position.x;
	posRect.y = m_position.y;
	posRect.w = m_frameWidth;
	posRect.h = m_frameHeight;

	tRectf clipRect;
	clipRect.x = m_frameWidth * col;
	clipRect.y = m_frameHeight * row;
	clipRect.w = m_frameWidth;
	clipRect.h = m_frameHeight;

	cGameRenderer::GetInstance()->DrawTexture( m_texture, clipRect, posRect );
}
