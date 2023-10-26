#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include <vector>

class cGameObject;

class cGameManager final
{
private:
	cGameManager();
	~cGameManager();

public:
	static cGameManager*	GetInstance();
	static void				DestroyInstance();

	bool					Init();
	void					Cleanup();

	void					Update( float deltaTime );
	void					Draw();

private:
	static cGameManager*	s_instance;

	std::vector<cGameObject*>
							m_gameObjects;
};

#endif