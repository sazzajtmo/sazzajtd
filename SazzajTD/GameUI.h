#ifndef GAME_UI_H
#define GAME_UI_H

struct SDL_Window;
struct SDL_Renderer;
union SDL_Event;

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

private:
	static	cGameUI*		s_instance;
};

#endif