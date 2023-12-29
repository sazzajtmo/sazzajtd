#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include <vector>
#include <memory>
#include "MemHelper.h"
#include "Utilities.h"
#include "GameDefs.h"

class cGameObject;
class cGameBoard;



class cGameManager final
{
private:
	cGameManager();
	~cGameManager();

public:
	static	cGameManager*					GetInstance();
	static	void							DestroyInstance();

			bool							Init();
			void							Cleanup();

			void							SpawnObject(eGameObjectTypes objectType, const tGameTransform& transform);
			void							DespawnObject( std::shared_ptr<cGameObject> gameObject );

			void							Update( float deltaTime );
			void							Draw();

	inline	std::shared_ptr<cGameBoard>		GetGameBoard()	{ return m_gameBoard; }
			
			//utilities
			void							GetGameObjectsInRadius(eGameObjectTypes objectType, std::vector<std::shared_ptr<cGameObject>>& objects, const tVector2Df& origin, float radius) const;
			[[nodiscard]]
			std::shared_ptr<cGameObject>	GetClosestGameObject(eGameObjectTypes objectType, const tVector2Df& origin, float maxRadius) const;

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

			float							m_spawnTimer		= 0.f;
			float							m_changeBoardTimer	= 10.f;
};

#endif