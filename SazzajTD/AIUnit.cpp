#include "AIUnit.h"
#include <cmath>
#include <algorithm>

#include "GameRenderer.h"
#include "AnimatedTexture.h"
#include "GameManager.h"
#include "GameBoard.h"
#include "GameLog.h"
#include "GameDefs.h"

const std::vector<const char*> g_aiModels = 
{
	"characters/green.png",
	"characters/pink.png",
	"characters/blue.png",
	"characters/yellow.png"
};

cAIUnit::cAIUnit()
: cGameObject(eGameObjectTypes::Enemy)
{
	m_transform.position.x = -300.f;
	m_transform.position.y = -300.f;
	

	m_targetPos.position = m_transform.position + tVector2Df( ( static_cast<float>(std::rand() % 200) - 100.f ) / 2.f, ( static_cast<float>(std::rand() % 200) - 100.f ) / 2.f );
}

cAIUnit::~cAIUnit()
{
}

void cAIUnit::Init()
{
	m_model = cAnimatedTexture::Load(g_aiModels[rand() % g_aiModels.size()]);
	
	if (m_model)
	{
		m_model->SetDimensions(1, 2);
		m_model->SetFramerate(15.f);
		m_model->SetPosition(m_transform.position);
		//m_model->SetScale(0.9f);
	}

	std::shared_ptr<cGameBoard> gameBoard = cGameManager::GetInstance()->GetGameBoard();

	if (gameBoard)
	{
		m_transform.position	= gameBoard->GetEntryPoint();
		m_pathToTarget			= gameBoard->FindPathAstar( m_transform.position, gameBoard->GetExitPoint() );
		m_currPathPointIdx		= m_pathToTarget.size() > 0u ? 0 : -1;
	}
}

void cAIUnit::Update(float deltaTime)
{
	cGameObject::Update(deltaTime);

	if (m_health <= 0.f)
	{
		cGameManager::GetInstance()->DespawnObject(shared_from_this());
		return;
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
			m_transform.position += dirToDest * m_speed * m_speedFactor * deltaTime;
		}
	}
	else
	{
		cGameManager::GetInstance()->DespawnObject( shared_from_this() );
	}

	m_speedFactor = 1.f; //reset speed factor every frame. other external factor may change it, but need to change it on frame.
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

	cGameObject::Draw();
}

void cAIUnit::ReceiveDamage(float dmgValue)
{
	m_health = std::clamp( dmgValue, 0.f, GameConfig::values.enemy_hp);
	
}
