#include "GameRenderer.h"
#include "SDL.h"
#include "SDL_surface.h"
#include "SDL_image.h"
#include "SDL_rect.h"
#include "MemHelper.h"
#include <algorithm>


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
		s_instance = snew cGameRenderer;

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

bool cGameRenderer::Init( SDL_Window* window, const tVector2Df& renderOffset )
{
	if( !window )
		return false;

	m_renderer.reset( SDL_CreateRenderer(window, 0, SDL_RENDERER_PRESENTVSYNC) );

	if (!m_renderer)
	{
		SDL_Log( "SDL_CreateRenderer failed %s", SDL_GetError() );
		return false;
	}

	SDL_GetWindowSize(window, &m_width, &m_height);
	m_renderOffset = renderOffset;

	//no background
	//SetBackground( "background.bmp" );
			
	return true;
}

void cGameRenderer::Cleanup()
{
	if( m_bgTexture )
		SDL_DestroyTexture( m_bgTexture );

	for( auto& customSurface : m_customSurfaces )
		SDL_FreeSurface( customSurface.second );
	m_customSurfaces.clear();
}

void cGameRenderer::RenderBegin()
{
	if( !m_renderer )
		return;

	SDL_SetRenderDrawColor(m_renderer.get(), 0, 0, 0, 255);
	SDL_RenderClear(m_renderer.get());
}

void cGameRenderer::Render()
{
	SDL_SetRenderDrawBlendMode(m_renderer.get(), SDL_BLENDMODE_BLEND);

	//SDL_Rect drawRect;
	//drawRect.x = 40;
	//drawRect.y = 40;
	//drawRect.w = 120;
	//drawRect.h = 80;

	//SDL_Rect clipRect;
	//clipRect.x = 40;
	//clipRect.y = 40;
	//clipRect.w = 120;
	//clipRect.h = 80;

	//if (m_bgTexture)
	//{
	//	SDL_Rect posRect;

	//	posRect.x = static_cast<int>(0			+ m_renderOffset.x);
	//	posRect.y = static_cast<int>(0			+ m_renderOffset.y);
	//	posRect.w = static_cast<int>(m_width	- m_renderOffset.x);
	//	posRect.h = static_cast<int>(m_height	- m_renderOffset.y);

	//	SDL_RenderCopy( m_renderer.get(), m_bgTexture, 0, &posRect);
	//}

	std::sort(m_renderedTextures.begin(), m_renderedTextures.end(), [](const tDrawTex& lh, const tDrawTex& rh) { return rh.prio > lh.prio; });

	SDL_Rect clipRect, posRect;

	for (const auto& tex : m_renderedTextures)
	{
		clipRect.x = static_cast<int>( tex.clip.x );
		clipRect.y = static_cast<int>( tex.clip.y );
		clipRect.w = static_cast<int>( tex.clip.w );
		clipRect.h = static_cast<int>( tex.clip.h );

		posRect.x = static_cast<int>( tex.pos.x + m_renderOffset.x );
		posRect.y = static_cast<int>( tex.pos.y + m_renderOffset.y );
		posRect.w = static_cast<int>( tex.pos.w );
		posRect.h = static_cast<int>( tex.pos.h );

		SDL_RenderCopy( m_renderer.get(), tex.tex, &clipRect, &posRect );
	}

	for (const auto& line : m_renderedLines)
	{
		SDL_SetRenderDrawColor(m_renderer.get(), static_cast<int>(line.color.r * 255.f), static_cast<int>(line.color.g * 255.f), static_cast<int>(line.color.b * 255.f), static_cast<int>(line.color.a * 255.f));
		SDL_RenderDrawLineF(m_renderer.get(), line.start.x + m_renderOffset.x, line.start.y + m_renderOffset.y, line.end.x + m_renderOffset.x, line.end.y + m_renderOffset.y);
	}

	SDL_Rect bitRect;

	for (const auto& bit : m_renderedBits)
	{
		bitRect.w = (int)bit.transform.scale * 4;
		bitRect.h = (int)bit.transform.scale * 4;
		bitRect.x = static_cast<int>(bit.transform.position.x - static_cast<float>(bitRect.w) * 0.5f + m_renderOffset.x);
		bitRect.y = static_cast<int>(bit.transform.position.y - static_cast<float>(bitRect.h) * 0.5f + m_renderOffset.y);

		SDL_SetRenderDrawColor(m_renderer.get(), static_cast<int>(bit.color.r * 255.f), static_cast<int>(bit.color.g * 255.f), static_cast<int>(bit.color.b * 255.f), static_cast<int>(bit.color.a * 255.f));
		SDL_RenderFillRect(m_renderer.get(), &bitRect);
	}

	m_renderedTextures.clear();
	m_renderedBits.clear();
	m_renderedLines.clear();
}

void cGameRenderer::RenderEnd()
{
	if( !m_renderer )
		return;

	SDL_RenderPresent(m_renderer.get());
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

void cGameRenderer::DrawTexture(SDL_Texture* tex, const tRectf& clip, const tRectf& pos, int prio)
{
	m_renderedTextures.emplace_back( tex, clip, pos, prio );
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

SDL_Texture* cGameRenderer::GetSDLTexture(const std::string& pathToFile)
{
	if( !m_renderer )
		return nullptr;

	SDL_Surface* surface = IMG_Load( pathToFile.c_str() );

	if( !surface )
		return nullptr;

	return SDL_CreateTextureFromSurface( m_renderer.get(), surface );
}

void cGameRenderer::SetBackground(const std::string& pathToFileTexture)
{
	if (m_bgTexture)
	{
		SDL_DestroyTexture( m_bgTexture );
		m_bgTexture = nullptr;
	}

	SDL_Surface* bgSurface = IMG_Load( pathToFileTexture.c_str() );

	if (bgSurface)
	{
		m_bgTexture = SDL_CreateTextureFromSurface( m_renderer.get(), bgSurface );
		SDL_FreeSurface( bgSurface );
	}
}

void cGameRenderer::ExportGridToFile(const std::vector<std::vector<int8_t>>& grid, int tileSize, const std::string& gridName)
{
	std::string gridTextureFileName			= gridName + ".png";
	std::string gridWalkableTextureFileName = gridName + "_WalkMask.png";

	const int numRows = (int) grid.size();
	const int numCols = (int) grid[0].size();

	SDL_Texture* walkableTexture = SDL_CreateTexture( m_renderer.get(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, tileSize * numCols, tileSize * numRows );

	SDL_SetRenderTarget(m_renderer.get(), walkableTexture);
	SDL_SetRenderDrawColor(m_renderer.get(), 0, 0, 0, 0);
	SDL_RenderClear(m_renderer.get());

	for (int y = 0; y < numRows; y++)
	{
		for (int x = 0; x < numCols; x++)
		{
			tColor		cellColor = grid[y][x] == 1 ? 0xffffffff : 0xff0f0f0f;
			SDL_Rect	bitRect;
			bitRect.w = tileSize;
			bitRect.h = tileSize;
			bitRect.x = tileSize * x;
			bitRect.y = tileSize * y;

			SDL_SetRenderDrawColor( m_renderer.get(), static_cast<int>( cellColor.r * 255.f ), static_cast<int>( cellColor.g * 255.f ), static_cast<int>( cellColor.b * 255.f ), static_cast<int>( cellColor.a * 255.f ) );
			SDL_RenderFillRect( m_renderer.get(), &bitRect );
		}
	}

	//save to texture part
	SDL_Surface* surface = SDL_CreateRGBSurface( 0, tileSize * numCols, tileSize * numRows, 32, 0, 0, 0, 0 );
	SDL_RenderReadPixels( m_renderer.get(), 0, surface->format->format, surface->pixels, surface->pitch );
	IMG_SavePNG( surface, gridWalkableTextureFileName.c_str() );
	
	SDL_SetRenderTarget(m_renderer.get(), nullptr);
	SDL_FreeSurface( surface );
	SDL_DestroyTexture( walkableTexture );
	//

	std::map<std::string, SDL_Texture*> roadTextures = 
	{
		{ "road_block", GetSDLTexture( "tiles/road_block.png" ) },
		{ "road_E",     GetSDLTexture( "tiles/road_E.png" ) },
		{ "road_ES",    GetSDLTexture( "tiles/road_ES.png" ) },
		{ "road_N",     GetSDLTexture( "tiles/road_N.png" ) },
		{ "road_NE",    GetSDLTexture( "tiles/road_NE.png" ) },
		{ "road_NES",   GetSDLTexture( "tiles/road_NES.png" ) },
		{ "road_NS",    GetSDLTexture( "tiles/road_NS.png" ) },
		{ "road_S",     GetSDLTexture( "tiles/road_S.png" ) },
		{ "road_W",     GetSDLTexture( "tiles/road_W.png" ) },
		{ "road_WE",    GetSDLTexture( "tiles/road_WE.png" ) },
		{ "road_WES",   GetSDLTexture( "tiles/road_WES.png" ) },
		{ "road_WN",    GetSDLTexture( "tiles/road_WN.png" ) },
		{ "road_WNE",   GetSDLTexture( "tiles/road_WNE.png" ) },
		{ "road_WNES",  GetSDLTexture( "tiles/road_WNES.png" ) },
		{ "road_WNS",   GetSDLTexture( "tiles/road_WNS.png" ) },
		{ "road_WS",    GetSDLTexture( "tiles/road_WS.png" ) }
	};

	SDL_Texture* simpleGrass = GetSDLTexture( "tiles/grass_1.png" );

	std::vector<SDL_Texture*> grassTextures = 
	{
		simpleGrass,
		GetSDLTexture( "tiles/grass_2.png" ),
		GetSDLTexture( "tiles/grass_3.png" )
	};

	const int numGrassTextures = (int) grassTextures.size();

	SDL_Texture* texture = SDL_CreateTexture( m_renderer.get(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, tileSize * numCols, tileSize * numRows );

	SDL_SetRenderTarget(m_renderer.get(), texture);
	SDL_SetRenderDrawColor(m_renderer.get(), 0, 0, 0, 255);
	SDL_SetRenderDrawBlendMode(m_renderer.get(), SDL_BLENDMODE_BLEND);
	SDL_RenderClear(m_renderer.get());

	SDL_Rect posRect;

	for (int y = 0; y < numRows; y++)
	{
		for (int x = 0; x < numCols; x++)
		{
			posRect.x = x * tileSize;
			posRect.y = y * tileSize;
			posRect.w = tileSize;
			posRect.h = tileSize;
			SDL_Texture* tileTexture = grid[y][x] == 0 && std::rand() % 100 > 80 ? grassTextures[rand()%numGrassTextures] : simpleGrass;
			SDL_RenderCopy( m_renderer.get(), tileTexture, nullptr, &posRect );
		}
	}

	const int numTrees = ( numRows * numCols ) / 5;

	std::vector<SDL_Texture*> decorationTextures = 
	{
		GetSDLTexture( "tiles/decoration_1.png" ),
		GetSDLTexture( "tiles/decoration_2.png" )
	};

	std::set<std::pair<int,int>> decorationPlacements;

	for (int i = 0; i < numTrees; i++)
	{
		const int x = std::rand() % numCols;
		const int y = std::rand() % numRows;
		auto pair = std::make_pair( x, y );

		if( decorationPlacements.count( pair ) > 0 || grid[y][x] != 0)
			continue;

		decorationPlacements.insert( pair );

		posRect.x = x * tileSize;
		posRect.y = y * tileSize;
		posRect.w = tileSize;
		posRect.h = tileSize;
		SDL_Texture* tileTexture = std::rand() % 100 > 80 ? decorationTextures[rand()%2] : decorationTextures[0];
		SDL_RenderCopy( m_renderer.get(), tileTexture, nullptr, &posRect );
	}

	for (int y = 0; y < (int)grid.size(); y++)
	{
		for (int x = 0; x < (int)grid[y].size(); x++)
		{
			if (grid[y][x] != 1)
				continue;

			std::string roadPrefix = "";

			if( x-1 >= 0 && grid[y][x-1] == 1 )
				roadPrefix += "W";
			if( y-1 >= 0 && grid[y-1][x] == 1)
				roadPrefix += "N";
			if (x+1 < (int) grid[y].size() && grid[y][x + 1] == 1)
				roadPrefix += "E";
			if (y + 1 < (int) grid.size() && grid[y + 1][x] == 1)
				roadPrefix += "S";

			SDL_Texture* tileTexture = roadPrefix.empty() ? grassTextures[rand()%3] : roadTextures["road_"+roadPrefix];

			posRect.x = x * tileSize;
			posRect.y = y * tileSize;
			posRect.w = tileSize;
			posRect.h = tileSize;

			SDL_RenderCopy( m_renderer.get(), tileTexture, nullptr, &posRect );
		}
	}

	//save to texture part
	SDL_Surface* surfaceRGB = SDL_CreateRGBSurface( 0, tileSize * numCols, tileSize * numRows, 32, 0, 0, 0, 0 );
	SDL_RenderReadPixels( m_renderer.get(), 0, surfaceRGB->format->format, surfaceRGB->pixels, surfaceRGB->pitch );
	IMG_SavePNG( surfaceRGB, gridTextureFileName.c_str() );
	
	SDL_SetRenderTarget(m_renderer.get(), nullptr);
	SDL_FreeSurface( surfaceRGB );
	SDL_DestroyTexture( texture );
	//
}

void cGameRenderer::DestroyRenderImplementation(SDL_Renderer* renderer)
{
	SDL_DestroyRenderer( renderer );
}

SDL_Renderer* cGameRenderer::GetRenderImplemention()
{
	return GetInstance()->m_renderer.get();
}
