#include "GameManager.h"
#include "GameObject.h"
#include "AIUnit.h"
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
	cGameBoard* gameBoard = new cGameBoard();
	gameBoard->InitPathfinding( 640, 480 ); //get size from renderer??
	m_gameObjects.push_back( gameBoard );
	
	m_gameObjects.push_back( new cAIUnit() );

	return true;
}

void cGameManager::Cleanup()
{
}

void cGameManager::Update(float deltaTime)
{
	for( auto& gameObject : m_gameObjects )
		gameObject->Update( deltaTime );
}

void cGameManager::Draw()
{
	for( auto& gameObject : m_gameObjects )
		gameObject->Draw();
}


