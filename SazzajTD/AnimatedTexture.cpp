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
}

std::unique_ptr<cAnimatedTexture> cAnimatedTexture::Load(const std::string& path)
{
	SDL_Texture* texture = cGameRenderer::GetInstance()->GetSDLTexture( path );

	if( !texture )
		return nullptr;

	std::unique_ptr<cAnimatedTexture> animTexture = std::make_unique<cAnimatedTexture>();

	animTexture->m_texture.reset(texture);
	animTexture->SetDimensions( 1, 1 );

	return std::move(animTexture);
}

void cAnimatedTexture::SetDimensions(int rows, int cols)
{
	m_rows = rows;
	m_cols = cols;

	int texWidth, texHeight;
	SDL_QueryTexture( m_texture.get(), nullptr, nullptr, &texWidth, &texHeight);

	m_frameWidth	= static_cast<float>( texWidth / m_cols );
	m_frameHeight	= static_cast<float>( texHeight / m_rows );
}

void cAnimatedTexture::SetFramerate(float framerate)
{
	m_framerate			= framerate;
	m_framerateDelta	= 1.f / m_framerate;
}

void cAnimatedTexture::SetPosition(const tVector2Df& position)
{
	m_position = position;
}

void cAnimatedTexture::SetPriority(int priority)
{
	m_priority = priority;
}

void cAnimatedTexture::GetFrameDims(float& frameWidth, float& frameHeight) const
{
	frameWidth	= m_frameWidth;
	frameHeight	= m_frameHeight;
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

	cGameRenderer::GetInstance()->DrawTexture(m_texture.get(), clipRect, posRect, m_priority);
}

void DestroyTexture(SDL_Texture* sdlTexture)
{
	if (sdlTexture)
		SDL_DestroyTexture(sdlTexture);
}
