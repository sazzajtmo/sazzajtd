#include "GameRenderer.h"
#include "SDL.h"
#include "SDL_surface.h"
#include "SDL_image.h"

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

	for (const auto& line : m_renderedLines)
	{
		SDL_SetRenderDrawColor( m_renderer, static_cast<int>( line.color.r * 255.f ), static_cast<int>( line.color.g * 255.f ), static_cast<int>( line.color.b * 255.f ), static_cast<int>( line.color.a * 255.f ) );
		SDL_RenderDrawLineF( m_renderer, line.start.x, line.start.y, line.end.x, line.end.y );
	}

	SDL_Rect bitRect;

	for (const auto& bit : m_renderedBits)
	{
		bitRect.w = (int) bit.transform.scale * 4;
		bitRect.h = (int) bit.transform.scale * 4;
		bitRect.x = (int) bit.transform.position.x - bitRect.w * 0.5f;
		bitRect.y = (int) bit.transform.position.y - bitRect.h * 0.5f;

		SDL_SetRenderDrawColor( m_renderer, static_cast<int>( bit.color.r * 255.f ), static_cast<int>( bit.color.g * 255.f ), static_cast<int>( bit.color.b * 255.f ), static_cast<int>( bit.color.a * 255.f ) );
		SDL_RenderFillRect( m_renderer, &bitRect );
		//SDL_RenderDrawPointF( m_renderer, bit.transform.position.x, bit.transform.position.y );
	}

	m_renderedBits.clear();
	m_renderedLines.clear();
}

void cGameRenderer::RenderEnd()
{
	if( !m_renderer )
		return;

	SDL_RenderPresent(m_renderer);
}

void cGameRenderer::DrawImmediate(const tGameTransform& transform, const tColor& color)
{
	m_renderedBits.emplace_back( transform, color );
}

void cGameRenderer::DrawImmediate(const tVector2Df& pos, const tColor& color)
{
	tGameTransform newTransform;
	newTransform.position = pos;

	DrawImmediate( newTransform, color );
}

void cGameRenderer::DrawLine(const tVector2Df& start, const tVector2Df& end, const tColor& color)
{
	m_renderedLines.emplace_back( start, end, color );
}

bool cGameRenderer::LoadCustomSurface(const std::string& pathToFile)
{
	std::map< std::string, SDL_Surface* >::iterator surfaceIt = m_customSurfaces.find( pathToFile );
	
	if( surfaceIt != m_customSurfaces.end() )
		return true;

	SDL_Surface* surface = IMG_Load( pathToFile.c_str() );

	if( !surface )
		return false;

	m_customSurfaces[pathToFile] = surface;

	return true;
}

void cGameRenderer::GetSurfaceSize( const std::string& surfaceName, int& w, int& h )
{
	auto surfaceIt = m_customSurfaces.find( surfaceName );
	
	if( surfaceIt == m_customSurfaces.end() )
		return;

	w = surfaceIt->second->w;
	h = surfaceIt->second->h;
}

tColor cGameRenderer::GetRGBA(const std::string& surfaceName, int x, int y) const
{
	auto surfaceIt = m_customSurfaces.find( surfaceName );
	
	if( surfaceIt == m_customSurfaces.end() )
		return tColor();

	SDL_Surface* surface = surfaceIt->second;

	auto getPixelValue = [](SDL_Surface* surface, int x, int y) -> Uint32
	{
		Uint8* pixel = (Uint8*) surface->pixels + y * surface->pitch + x * surface->format->BytesPerPixel;

		switch (surface->format->BytesPerPixel)
		{
			case 1:
				return *pixel;
			break;

			case 2:
				return *(Uint16 *)pixel;
			break;

			case 3:
				if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
				return pixel[0] << 16 | pixel[1] << 8 | pixel[2];
				else
				return pixel[0] | pixel[1] << 8 | pixel[2] << 16;
			break;

			case 4:
				return *(Uint32 *)pixel;
			break;

			default:
				break;
		}

		return 0;
	};

	SDL_LockSurface( surface );

	unsigned char r, g, b, a;
	SDL_GetRGBA( getPixelValue( surface, x, y ), surface->format, &r, &g, &b, &a );

	SDL_UnlockSurface( surface );

	tColor color;
	color.r = static_cast<float>( r ) / 256.f;
	color.g = static_cast<float>( g ) / 256.f;
	color.b = static_cast<float>( b ) / 256.f;
	color.a = static_cast<float>( a ) / 256.f;

	return color;
}
