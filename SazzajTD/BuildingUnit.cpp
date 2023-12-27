#include "BuildingUnit.h"

#include "GameRenderer.h"
#include "AnimatedTexture.h"
#include "GameManager.h"
#include "GameBoard.h"
#include "GameLog.h"

const std::vector<const char*> g_aiModels = 
{
	"characters/green.png",
	"characters/pink.png",
	"characters/blue.png",
	"characters/yellow.png"
};

cBuildingUnit::cBuildingUnit()
{
	m_transform.position.x = -300.f;
	m_transform.position.y = -300.f;
}

cBuildingUnit::~cBuildingUnit()
{
}

void cBuildingUnit::Init()
{
	m_model = cAnimatedTexture::Load(g_aiModels[rand() % g_aiModels.size()]);
	
	if (m_model)
	{
		m_model->SetDimensions(1, 2);
		m_model->SetFramerate(15.f);
		m_model->SetPosition(m_transform.position);
	}

	std::shared_ptr<cGameBoard> gameBoard = cGameManager::GetInstance()->GetGameBoard();

	//if (gameBoard)
	//{
	//	m_transform.position	= gameBoard->GetEntryPoint();
	//	m_pathToTarget			= gameBoard->FindPathAstar( m_transform.position, gameBoard->GetExitPoint() );
	//	m_currPathPointIdx		= m_pathToTarget.size() > 0u ? 0 : -1;
	//}
}

void cBuildingUnit::Update(float deltaTime)
{
	cGameObject::Update(deltaTime);
}

void cBuildingUnit::Draw()
{
	cGameObject::Draw();
}
