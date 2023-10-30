#include "GameManager.h"
#include "GameObject.h"
#include "AIUnit.h"
#include "PlayerUnit.h"
#include "GameBoard.h"

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
		s_instance = new cGameManager;

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
	m_gameBoard.reset( new cGameBoard() );
	m_gameBoard->InitPathfinding( 640, 480 ); //get size from renderer??

	SpawnObject<cAIUnit>();
	SpawnObject<cPlayerUnit>();

	return true;
}

void cGameManager::Cleanup()
{
	for (auto& gameObject : m_gameObjects)
	{
		delete gameObject;
	}

	m_gameObjects.clear();

	m_gameBoard.reset();
}

void cGameManager::Update(float deltaTime)
{
	if( m_gameBoard )
		m_gameBoard->Update( deltaTime );

	for( auto& gameObject : m_gameObjects )
		gameObject->Update( deltaTime );
}

void cGameManager::Draw()
{
	if( m_gameBoard )
		m_gameBoard->Draw();

	for( auto& gameObject : m_gameObjects )
		gameObject->Draw();
}


