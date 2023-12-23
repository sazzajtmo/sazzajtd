#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include <vector>
#include <memory>
#include "MemHelper.h"

class cGameObject;
class cGameBoard;

class cGameManager final
{
private:
	cGameManager();
	~cGameManager();

public:
	static	cGameManager*				GetInstance();
	static	void							DestroyInstance();

			bool							Init();
			void							Cleanup();

			template<class T>
			void							SpawnObject()
			{
				cGameObject* gameObject = snew T;

				gameObject->Init();

				m_gameObjects.push_back( gameObject );
			}
			void							DespawnObject( cGameObject* gameObject );

			void							Update( float deltaTime );
			void							Draw();

	inline	std::shared_ptr<cGameBoard>		GetGameBoard()	{ return m_gameBoard; }

private:
	static	cGameManager*					s_instance;

			std::vector<cGameObject*>		m_gameObjects;
			std::vector<cGameObject*>		m_despawnList;

			std::shared_ptr<cGameBoard>		m_gameBoard;

			float							m_spawnTimer		= 0.f;
			float							m_changeBoardTimer	= 10.f;
};

#endif