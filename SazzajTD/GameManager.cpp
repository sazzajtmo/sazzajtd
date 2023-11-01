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
	GameBoardGenerator::CreateGameBoard( 24, 20, 27, std::rand() % 10 );

	m_gameBoard.reset( snew cGameBoard() );
	m_gameBoard->InitPathfinding( GENERATED_WALKABLE_TEXTURE_FILE_PATH );

	cGameRenderer::GetInstance()->SetBackground( GENERATED_BOARD_TEXTURE_FILE_PATH );

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

	if (m_gameBoard.get())
	{
		m_gameBoard->Cleanup();
		m_gameBoard.reset();
	}
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


