#ifndef GAME_RENDERER_H
#define GAME_RENDERER_H

#include <vector>
#include <map>
#include <set>
#include <string>
#include <memory>
#include "GameDefs.h"

struct SDL_Renderer;
struct SDL_Window;
struct SDL_Surface;
struct SDL_Texture;

class cGameRenderer final
{
private:
	struct tDrawBit
	{
		tGameTransform	transform;
		tColor			color;

		explicit tDrawBit( tGameTransform _transform, tColor _color ) : transform( _transform ), color( _color ) {}
	};

	struct tDrawLine
	{
		tVector2Df		start;
		tVector2Df		end;
		tColor			color;

		explicit tDrawLine( tVector2Df _start, tVector2Df _end, tColor _color ) : start( _start ), end( _end ), color( _color ) {}
	};

	struct tDrawTex
	{
		SDL_Texture*	tex		= nullptr;
		tRectf			clip;
		tRectf			pos;
		int				prio	= 1;

		explicit tDrawTex( SDL_Texture* _tex, tRectf _clip, tRectf _pos, int _prio ) : tex( _tex ), clip( _clip ), pos( _pos ), prio( _prio ) {}
	};

	cGameRenderer();
	~cGameRenderer();

public:
	static	cGameRenderer*			GetInstance();
	static	void					DestroyInstance();

			bool					Init( SDL_Window* window, const tVector2Df& renderOffset );
			void					Cleanup();

			void					RenderBegin();
			void					Render();
			void					RenderEnd();

			void					DrawImmediate( const tGameTransform& transform, const tColor& color );
			void					DrawImmediate( const tVector2Df& pos, const tColor& color );
			void					DrawLine( const tVector2Df& start, const tVector2Df& end, const tColor& color );
			void					DrawTexture( SDL_Texture* tex, const tRectf& clip, const tRectf& pos, int prio = 1 );

			bool					LoadCustomSurface( const std::string& pathToFile );
			void					GetSurfaceSize( const std::string& surfaceName, int& w, int& h );
			tColor					GetRGBA( const std::string& surfaceName, int x, int y ) const;

			SDL_Texture*			GetSDLTexture( const std::string& pathToFile );
			void					SetBackground( const std::string& pathToFileTexture );

			void					ExportGridToFile( const std::vector<std::vector<int8_t>>& grid, int tileSize, const std::string& gridName );

	static	void					DestroyRenderImplementation( SDL_Renderer* renderer );
	static	SDL_Renderer*			GetRenderImplemention();	//bleah

	inline	const tVector2Df&		GetRenderOffset() const { return m_renderOffset; }

private:
	static	cGameRenderer*			s_instance;

			std::unique_ptr<SDL_Renderer, decltype(&DestroyRenderImplementation)>			
									m_renderer { nullptr, DestroyRenderImplementation };

			//TODO merge these together
			std::vector<tDrawBit>	m_renderedBits;
			std::vector<tDrawLine>	m_renderedLines;
			std::vector<tDrawTex>	m_renderedTextures;

			std::map< std::string, SDL_Surface* >
									m_customSurfaces;

			int						m_width			= 0;
			int						m_height		= 0;
			tVector2Df				m_renderOffset{ 0.f };

			SDL_Texture*			m_bgTexture		= nullptr;
};

#endif