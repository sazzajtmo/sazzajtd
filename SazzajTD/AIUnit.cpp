#include "AIUnit.h"
#include <math.h>

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

cAIUnit::cAIUnit()
{
	m_transform.position.x = 300.f;
	m_transform.position.y = 300.f;

	m_targetPos.position = m_transform.position + tVector2Df( ( static_cast<float>(std::rand() % 200) - 100.f ) / 2.f, ( static_cast<float>(std::rand() % 200) - 100.f ) / 2.f );
}

cAIUnit::~cAIUnit()
{
	if( m_model )
		cAnimatedTexture::Destroy( m_model );
}

void cAIUnit::Init()
{
	m_model = cAnimatedTexture::Load(g_aiModels[rand() % g_aiModels.size()]);
	
	if( m_model )
		m_model->SetDimensions(1, 2)->SetFramerate(15.f);

	std::shared_ptr<cGameBoard> gameBoard = cGameManager::GetInstance()->GetGameBoard();

	if (gameBoard)
	{
		m_transform.position	= gameBoard->GetEntryPoint();
		m_pathToTarget			= gameBoard->FindPathAstar( m_transform.position, gameBoard->GetExitPoint() );

		if( m_pathToTarget.size() == 0u )
			int x = 1;

		m_currPathPointIdx		= m_pathToTarget.size() > 0u ? 0 : -1;
	}
}

void cAIUnit::Update(float deltaTime)
{
	if (m_model)
	{
		m_model->Update( deltaTime );
		float modelW, modelH;
		m_model->GetFrameDims( modelW, modelH );
		m_model->SetPosition( m_transform.position - tVector2Df( modelW * 0.5f, modelH * 0.5f ) );
	}

#ifdef RANDOM_MOVEMENT
	if (std::abs(distance(m_transform.position, m_targetPos.position)) < 1.1f)
	{
		m_targetPos.position = m_transform.position + tVector2Df( ( static_cast<float>(std::rand() % 200) - 100.f ) / 2.f, ( static_cast<float>(std::rand() % 200) - 100.f ) / 2.f );
		//m_speed = 100.f;//static_cast<float>( std::rand() % 100 );
	}

	tVector2Df dirToDest = directionNormalized( m_transform.position, m_targetPos.position );

	m_transform.position += dirToDest * m_speed * deltaTime;
#endif

	if( m_currPathPointIdx >= 0 && m_currPathPointIdx < (int)m_pathToTarget.size() )
	{
		const tVector2Df& targetPosition = m_pathToTarget[m_currPathPointIdx];

		if (std::abs(distance(m_transform.position, targetPosition)) < 1.1f)
		{
			m_currPathPointIdx++;
		}
		else
		{
			tVector2Df dirToDest = directionNormalized( m_transform.position, targetPosition );
			m_transform.position += dirToDest * m_speed * deltaTime;
		}
	}
	else
	{
		cGameManager::GetInstance()->DespawnObject( this );
	}
}

void cAIUnit::Draw()
{
	//cGameRenderer::GetInstance()->DrawImmediate( m_targetPos, 0xff00ff00 );

	//for ( int i = 1; i < (int) m_pathToTarget.size(); i++ )
	//{
	//	cGameRenderer::GetInstance()->DrawImmediate( m_pathToTarget[i-1], 0xff0000ff );
	//	cGameRenderer::GetInstance()->DrawLine( m_pathToTarget[i-1], m_pathToTarget[i], 0xff0000ff );
	//}

	//if (m_pathToTarget.size() > 1)
	//{
	//	cGameRenderer::GetInstance()->DrawImmediate( m_pathToTarget[0], 0xffff0000);
	//	cGameRenderer::GetInstance()->DrawImmediate( m_pathToTarget[m_pathToTarget.size()-1], 0xffff0000);
	//}

	if( m_model )
		m_model->Draw();
}
