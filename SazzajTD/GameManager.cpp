#include "GameManager.h"
#include "GameObject.h"
#include "AIUnit.h"
#include "PlayerUnit.h"
#include "GameBoard.h"
#include "GameBoardGenerator.h"
#include "GameRenderer.h"
#include "MemHelper.h"

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
	

	//SpawnObject<cAIUnit>();
	//SpawnObject<cPlayerUnit>();

	return true;
}

void cGameManager::Cleanup()
{
	for (auto& gameObject : m_gameObjects)
	{
		delete gameObject;
	}

	m_gameObjects.clear();

	if (m_gameBoard.get())
	{
		m_gameBoard->Cleanup();
		m_gameBoard.reset();
	}
}

void cGameManager::DespawnObject(cGameObject* gameObject)
{
	//should use index rather than pointers
	m_despawnList.push_back( gameObject );
}

void cGameManager::Update(float deltaTime)
{
	m_changeBoardTimer -= deltaTime;

	if (m_changeBoardTimer <= 0.f)
	{
		m_changeBoardTimer = 10.f;
		//Init();
		return;
	}

	//should use index rather than pointers
	for (auto& gameObject : m_despawnList)
	{
		auto gameObjIt = std::find( m_gameObjects.begin(), m_gameObjects.end(), gameObject );

		if (gameObjIt != m_gameObjects.end())
		{
			m_gameObjects.erase(gameObjIt);
			gameObject->Cleanup();
			delete gameObject;
		}
	}

	m_despawnList.clear();

	if( m_gameBoard )
		m_gameBoard->Update( deltaTime );

	for( auto& gameObject : m_gameObjects )
		gameObject->Update( deltaTime );


	m_spawnTimer -= deltaTime;

	if (m_spawnTimer <= 0.f)
	{
		m_spawnTimer = 1.f;

		SpawnObject<cAIUnit>();
	}
}

void cGameManager::Draw()
{
	if( m_gameBoard )
		m_gameBoard->Draw();

	for( auto& gameObject : m_gameObjects )
		gameObject->Draw();
}


