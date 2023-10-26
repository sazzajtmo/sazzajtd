#ifndef GAME_RENDERER_H
#define GAME_RENDERER_H

#include <vector>
#include <memory>
#include "GameDefs.h"

struct SDL_Renderer;
struct SDL_Window;
struct SDL_Surface;
struct SDL_Texture;

class cGameRenderer final
{
private:
	cGameRenderer();
	~cGameRenderer();

public:
	static cGameRenderer*	GetInstance();
	static void				DestroyInstance();

	bool					Init( SDL_Window* window );
	void					Cleanup();

	void					RenderBegin();
	void					Render();
	void					RenderEnd();

	void					DrawImmediate( const tGameTransform& transform );

private:
	static cGameRenderer*	s_instance;

	SDL_Renderer*			m_renderer		= nullptr;

	std::vector<tGameTransform>
							m_renderedBits;

	SDL_Texture*			m_bgTexture		= nullptr;
};

#endif