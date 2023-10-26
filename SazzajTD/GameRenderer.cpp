#include "GameRenderer.h"
#include "SDL.h"
#include "SDL_surface.h"

cGameRenderer* cGameRenderer::s_instance( nullptr );

cGameRenderer::cGameRenderer()
{
}

cGameRenderer::~cGameRenderer()
{
}

cGameRenderer* cGameRenderer::GetInstance()
{
	if( !s_instance )
		s_instance = new cGameRenderer;

	return s_instance;
}

void cGameRenderer::DestroyInstance()
{
	if (s_instance)
	{
		s_instance->Cleanup();
		delete s_instance;
		s_instance = nullptr;
	}
}

bool cGameRenderer::Init( SDL_Window* window )
{
	if( !window )
		return false;

	m_renderer = SDL_CreateRenderer( window, 0, SDL_RENDERER_PRESENTVSYNC );

	if (!m_renderer)
	{
		SDL_Log( "SDL_CreateRenderer failed %s", SDL_GetError() );
		return false;
	}

	SDL_Surface* bgSurface = SDL_LoadBMP( "background.bmp" );

	if( bgSurface )
		m_bgTexture = SDL_CreateTextureFromSurface( m_renderer, bgSurface );

	return true;
}

void cGameRenderer::Cleanup()
{
	SDL_DestroyRenderer( m_renderer );

	if( m_bgTexture )
		SDL_DestroyTexture( m_bgTexture );
}

void cGameRenderer::RenderBegin()
{
	if( !m_renderer )
		return;

	SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
	SDL_RenderClear(m_renderer);
}

void cGameRenderer::Render()
{
	if (m_bgTexture)
	{
		SDL_RenderCopy( m_renderer, m_bgTexture, nullptr, nullptr );
	}

	SDL_Rect bitRect;

	for (const auto& bit : m_renderedBits)
	{
		bitRect.x = (int) bit.position.x;
		bitRect.y = (int) bit.position.y;
		bitRect.w = (int) bit.scale * 10;
		bitRect.h = (int) bit.scale * 10;

		SDL_SetRenderDrawColor( m_renderer, 0, 0, 255, 255 );
		SDL_RenderFillRect( m_renderer, &bitRect );
	}

	m_renderedBits.clear();
}

void cGameRenderer::RenderEnd()
{
	if( !m_renderer )
		return;

	SDL_RenderPresent(m_renderer);
}

void cGameRenderer::DrawImmediate(const tGameTransform& transform)
{
	m_renderedBits.push_back( transform );
}
