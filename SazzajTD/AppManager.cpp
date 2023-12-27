//#include "framework.h"
#include "AppManager.h"
#include "SDL.h"
#include "SDL_main.h"
#include <cmath>
#include <ctime>
#include "GameManager.h"
#include "GameRenderer.h"
#include "GameInputManager.h"
#include "GameUI.h"
#include "MemHelper.h"

#pragma comment( lib, "SDL2.lib" )
#pragma comment( lib, "SDL2_image.lib" )

cAppManager* cAppManager::s_instance( nullptr );

cAppManager::cAppManager()
{
}

cAppManager::~cAppManager()
{
}

cAppManager* cAppManager::GetInstance()
{
	if( !s_instance )
		s_instance = snew cAppManager;

	return s_instance;
}

void cAppManager::DestroyInstance()
{
	if (s_instance)
	{
		s_instance->Cleanup();
		delete s_instance;
		s_instance = nullptr;
		return;
	}
}

bool cAppManager::Init()
{
	std::srand( (uint32_t) std::time( 0 ) );

	if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
	{
		SDL_Log("SDL init failed %s", SDL_GetError());
		return false;
	}

	const int topMenuOffset = 30;
	const int width			= 648;
	const int height		= 480 + topMenuOffset;

	m_window = SDL_CreateWindow( m_appName.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);

	if( !m_window )
	{ 
		SDL_Log( "SDL_CreateWindow failed %s", SDL_GetError() );
		return false;
	}

	cGameInputManager::GetInstance();
	cGameRenderer::GetInstance()->Init( m_window, tVector2Df( 0.f, topMenuOffset ) );

	if( !cGameManager::GetInstance()->Init() )
	{
		SDL_Log( "cGameManager::GetInstance()->Init() failed");
		return false;
	}

	cGameUI::GetInstance()->Init(m_window, cGameRenderer::GetInstance()->GetRenderImplemention());
		
	return true;
}

void cAppManager::Cleanup()
{
	cGameUI::DestroyInstance();
	cGameManager::DestroyInstance();
	cGameInputManager::DestroyInstance();
	cGameRenderer::DestroyInstance();
	SDL_DestroyWindow( m_window );
	SDL_Quit();
}

void cAppManager::MainLoop()
{
	SDL_Rect mouseposrect;
	SDL_bool keep_going = SDL_TRUE;
	SDL_Event event;

	mouseposrect.x = mouseposrect.y = -1000;  /* -1000 so it's offscreen at start */
	mouseposrect.w = mouseposrect.h = 50;

	Uint64 lastTick = SDL_GetTicks64();

	/* run the program until told to stop. */
	while (keep_going) {

		/* run through all pending events until we run out. */
		cGameUI::GetInstance()->InputBegin();

		while (SDL_PollEvent(&event)) 
		{
			cGameUI::GetInstance()->InputHandle(&event);

			switch (event.type) 
			{
				case SDL_EventType::SDL_QUIT:  /* triggers on last window close and other things. End the program. */
					keep_going = SDL_FALSE;
				break;

				case SDL_EventType::SDL_KEYDOWN:  /* quit if user hits ESC key */
					if (event.key.keysym.sym == SDLK_ESCAPE) 
					{
						keep_going = SDL_FALSE;
					}
				break;

				case SDL_EventType::SDL_MOUSEBUTTONUP:  /* keep track of the latest mouse position */
					/* center the square where the mouse is */
					//SDL_Log( "hello %d", event.button.button );
					mouseposrect.x = event.motion.x - (mouseposrect.w / 2);
					mouseposrect.y = event.motion.y - (mouseposrect.h / 2);

					cGameInputManager::GetInstance()->FeedMouseEvent( event.button.button, event.motion.x, event.motion.y );
				break;

				case SDL_EventType::SDL_MOUSEMOTION:  /* keep track of the latest mouse position */
					/* center the square where the mouse is */
					mouseposrect.x = event.motion.x - (mouseposrect.w / 2);
					mouseposrect.y = event.motion.y - (mouseposrect.h / 2);

					cGameInputManager::GetInstance()->FeedMouseEvent(event.button.button, event.motion.x, event.motion.y);
				break;
			}
		}
		cGameUI::GetInstance()->InputEnd();


		Uint64	deltaTicks	= SDL_GetTicks64() - lastTick;
		float	deltaTime	= deltaTicks / 1000.f;

		cGameManager::GetInstance()->Update( deltaTime );
		cGameManager::GetInstance()->Draw();
		//SDL_Log( "delta %0.3f", deltaTime );

		cGameUI::GetInstance()->Update(deltaTime);

		cGameRenderer::GetInstance()->RenderBegin();
		cGameRenderer::GetInstance()->Render();
		cGameUI::GetInstance()->Draw();
		cGameRenderer::GetInstance()->RenderEnd();		

		lastTick = SDL_GetTicks64();

		SDL_Delay( 16 );
	}	
}