#include "GameManager.h"
#include "Utilities.h"
#include "GameObject.h"
#include "AIUnit.h"
#include "PlayerUnit.h"
#include "BuildingUnit.h"
#include "GunBuildingUnit.h"
#include "StaticUnit.h"
#include "GameBoard.h"
#include "GameRenderer.h"
#include "GameScoreManager.h"
#include "MemHelper.h"
#include <algorithm>

cGameManager* cGameManager::s_instance( nullptr );

cGameManager::cGameManager()
{
}

cGameManager::~cGameManager()
{
}

cGameManager* cGameManager::GetInstance()
{
	if( !s_instance )
		s_instance = snew cGameManager;

	return s_instance;
}

const cGameManager* cGameManager::GetConstInstance()
{
	return GetInstance();
}

void cGameManager::DestroyInstance()
{
	if (s_instance)
	{
		s_instance->Cleanup();
		delete s_instance;
		s_instance = nullptr;
	}
}

bool cGameManager::Init()
{

	m_scoreMgr.reset();
	m_scoreMgr = std::make_shared<cGameScoreManager>();

	for (auto& gameObject : m_gameObjects)
	{
		DespawnObject( gameObject );
	}

	if (m_gameBoard.get())
	{
		m_gameBoard->Cleanup();
		m_gameBoard.reset();
	}

	m_gameBoard = std::make_unique<cGameBoard>();
	m_gameBoard->Init();
	
	m_numBuildingsLeft = GameConfig::values.buildings;

	//SpawnObject<cAIUnit>();
	//SpawnObject<cPlayerUnit>();

	return true;
}

bool cGameManager::Reset()
{
	return Init();
}

void cGameManager::Cleanup()
{
	for (auto& gameObject : m_gameObjects)
	{
		gameObject->Cleanup();
	}

	m_gameObjects.clear();
	m_despawnList.clear();

	if (m_gameBoard)
	{
		m_gameBoard->Cleanup();
	}
}

std::shared_ptr<cGameObject> cGameManager::SpawnObject(eGameObjectTypes objectType, const tGameTransform& transform)
{
	std::shared_ptr<cGameObject> newObject;

	switch (objectType)
	{
		case eGameObjectTypes::Static:
			newObject = SpawnObject<cStaticUnit>();
		break;

		case eGameObjectTypes::Enemy:
			newObject = SpawnObject<cAIUnit>();
			break;

		case eGameObjectTypes::Building:
			newObject = SpawnObject<cBuildingUnit>();
			break;

		case eGameObjectTypes::GunBuilding:
			newObject = SpawnObject<cGunBuildingUnit>();
			break;

		default:
		case eGameObjectTypes::Player:
		case eGameObjectTypes::None:
			break;
	}

	if (newObject)
		newObject->SetPosition(transform.position);

	return newObject;
}

void cGameManager::DespawnObject(std::shared_ptr<cGameObject> gameObject)
{
	//should use index rather than pointers
	m_despawnList.push_back( std::move(gameObject) );
}

void cGameManager::Update(float deltaTime)
{
	if( m_gameState != eGameState::Playing )
		return;

	//should use index rather than pointers
	for (auto& gameObject : m_despawnList)
	{
		auto gameObjIt = std::find( m_gameObjects.begin(), m_gameObjects.end(), gameObject );

		if (gameObjIt != m_gameObjects.end())
		{
			m_gameObjects.erase(gameObjIt);
		}
	}

	m_despawnList.clear();

	if( m_gameBoard )
		m_gameBoard->Update( deltaTime );

	const size_t numGameObjects = m_gameObjects.size();

	for( size_t i = 0; i < numGameObjects; i++ )
		m_gameObjects[i]->Update(deltaTime);	//new game objects may spawn here

	m_spawnTimer -= deltaTime;

	if (m_spawnTimer <= 0.f)
	{
		m_spawnTimer = 1.f / GameConfig::values.enemy_spawn_rate;

		SpawnObject<cAIUnit>();
	}

	m_scoreMgr->Update(deltaTime);
}

void cGameManager::Draw()
{
	if (m_gameBoard)
	{
		m_gameBoard->Draw();
		m_gameBoard->DrawDebug();
	}

	for (auto& gameObject : m_gameObjects)
	{
		gameObject->Draw();
		gameObject->DrawDebug();
	}
}

void cGameManager::GetGameObjectsInRadius(eGameObjectTypes objectType, std::vector<std::shared_ptr<cGameObject>>& objects, const tVector2Df& origin, float radius) const
{
	for (const auto& object : m_gameObjects)
	{
		if (object->GetType() == objectType && distance(origin, object->GetPosition()) <= radius)
			objects.push_back(object);
	}
}

std::shared_ptr<cGameObject> cGameManager::GetClosestGameObject(eGameObjectTypes objectType, const tVector2Df& origin, float maxRadius) const
{
	float	minDistance = -1.f;
	int		minIndex	= -1;

	for (size_t i = 0u; i < m_gameObjects.size(); i++)
	{
		const auto& gameObject = m_gameObjects[i];

		if (gameObject->GetType() != objectType)
			continue;

		float distToOrigin = distance(origin, gameObject->GetPosition());

		if (distToOrigin <= maxRadius && distToOrigin > minDistance)
		{
			minDistance	= distToOrigin;
			minIndex	= static_cast<int>(i);
		}
	}

	return minIndex != -1 ? m_gameObjects[minIndex] : nullptr;
}

std::shared_ptr<cGameScoreManager> cGameManager::GetScoreManager() const
{
	return m_scoreMgr;
}

eGameObjectTypes cGameManager::GetCurrentBuildingType()
{
	eGameObjectTypes nextBuilding = m_nextBuilding;

	m_numBuildingsLeft--;
	const int numBuildingTypes = static_cast<int>(eGameObjectTypes::Buildings_End) - static_cast<int>(eGameObjectTypes::Buildings_Start) + 1;
	m_nextBuilding = static_cast<eGameObjectTypes>(static_cast<int>(eGameObjectTypes::Buildings_Start) + ( std::rand() % numBuildingTypes ) );

	return nextBuilding;
}

eGameObjectTypes cGameManager::GetCurrentBuildingType() const
{
	return m_nextBuilding;
}


