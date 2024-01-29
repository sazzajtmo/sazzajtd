#ifndef GAME_UI_H
#define GAME_UI_H

#include "GameDefs.h"

struct SDL_Window;
struct SDL_Renderer;
union SDL_Event;
struct nk_image;

#include <unordered_map>
#include <string>
#include <memory>

class cGameUI final
{
private:
	cGameUI();
	virtual ~cGameUI();

public:
	static	cGameUI*		GetInstance();
	static	void			DestroyInstance();

			void			Init( SDL_Window* sdlWindow, SDL_Renderer* sdlRenderer );
			void			Cleanup();

			void			InputBegin();
			void			InputHandle(SDL_Event* sdlEvent);
			void			InputEnd();
			void			Update( float deltaTime );
			void			Draw();

			void			UpdateStartUp(float deltaTime);
			void			UpdatePaused(float deltaTime);
			void			UpdatePlaying(float deltaTime);

private:
	static	cGameUI*		s_instance;

			int				m_renderW = 0;
			int				m_renderH = 0;

			std::unordered_map<std::string, std::unique_ptr<struct nk_image>>
							m_images;
			int				m_gameBoardType{ static_cast<int>(eGameBoardType::SimpleLoops)};
};

#endif