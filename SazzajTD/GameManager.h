#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include <vector>
#include <memory>
#include "MemHelper.h"
#include "Utilities.h"
#include "GameDefs.h"

class cGameObject;
class cGameBoard;
class cGameScoreManager;

class cGameManager final
{
private:
	cGameManager();
	~cGameManager();

public:
	static	cGameManager*					GetInstance();
	static	void							DestroyInstance();

			bool							Init();
			bool							Reset();
			void							Cleanup();

			std::shared_ptr<cGameObject>	SpawnObject(eGameObjectTypes objectType, const tGameTransform& transform);
			void							DespawnObject( std::shared_ptr<cGameObject> gameObject );

			void							Update( float deltaTime );
			void							Draw();

	inline	std::shared_ptr<cGameBoard>		GetGameBoard() const					{ return m_gameBoard; }

			//TODO change this to a get and switchTo that has a Begin/End functions for states
			//works for now
	inline	eGameState						GetGameState() const					{ return m_gameState; }
	inline	void							SetGameState( eGameState gameState )	{ m_gameState = gameState; }
			
			//utilities
			void							GetGameObjectsInRadius(eGameObjectTypes objectType, std::vector<std::shared_ptr<cGameObject>>& objects, const tVector2Df& origin, float radius) const;
			[[nodiscard]]
			std::shared_ptr<cGameObject>	GetClosestGameObject(eGameObjectTypes objectType, const tVector2Df& origin, float maxRadius) const;
			[[nodiscard]]
			std::shared_ptr<cGameScoreManager>
											GetScoreManager() const;

private:
			template<class T>
			std::shared_ptr<cGameObject>	SpawnObject()
			{
				std::shared_ptr<cGameObject> gameObject = std::make_shared<T>();

				gameObject->Init();

				m_gameObjects.push_back(std::move(gameObject));

				return m_gameObjects.back();
			}

private:
	static	cGameManager*					s_instance;

			std::vector<std::shared_ptr<cGameObject>>	
											m_gameObjects;
			std::vector<std::shared_ptr<cGameObject>>
											m_despawnList;

			std::shared_ptr<cGameBoard>		m_gameBoard;
			std::shared_ptr<cGameScoreManager>
											m_scoreMgr;

			eGameState						m_gameState			= eGameState::StartUp; //TODO change this to a get and switchTo that has a Begin/End functions for states

			float							m_spawnTimer		= 1.f;
};

#endif