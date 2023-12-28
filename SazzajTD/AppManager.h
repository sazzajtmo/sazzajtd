#ifndef APP_MANAGER_H
#define APP_MANAGER_H

#include <string>
#include <vector>
#include "Utilities.h"

struct SDL_Window;
struct SDL_Renderer;

class cAppManager final
{
private:
	cAppManager();
	~cAppManager();

public:
	static cAppManager*		GetInstance();
	static void				DestroyInstance();

	bool					Init();
	void					Cleanup();
	void					MainLoop();

private:
	SDL_Window*				m_window		= nullptr;
	std::string				m_appName		= "Sazzaj Tower Defence";

	static cAppManager*		s_instance;
};

#endif
