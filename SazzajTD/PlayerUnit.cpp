#include "PlayerUnit.h"
#include <math.h>

#include "GameRenderer.h"
#include "GameInputManager.h"
#include "GameManager.h"
#include "GameBoard.h"
#include "AnimatedTexture.h"

cPlayerUnit::cPlayerUnit()
{
}

cPlayerUnit::~cPlayerUnit()
{
}

void cPlayerUnit::Init()
{
	m_mouseCbHandle = cGameInputManager::GetInstance()->RegisterForMouseEvent([this](const cGameInputManager::tMouseEventData& mouseEvent)
	{
		if (mouseEvent.button == cGameInputManager::eMouseButton::Left)
		{
			m_transform.position.x = static_cast<float>(mouseEvent.x);
			m_transform.position.y = static_cast<float>(mouseEvent.y);
		}
		else if (mouseEvent.button == cGameInputManager::eMouseButton::Right)
		{
			m_targetPos.x = static_cast<float>(mouseEvent.x);
			m_targetPos.y = static_cast<float>(mouseEvent.y);
		}
	});

	m_model = cAnimatedTexture::Load( "fly.png" );
	
	if( m_model )
		m_model->SetDimensions(2, 3)->SetFramerate(15.f);
}

void cPlayerUnit::Update(float deltaTime)
{
	if (m_model)
	{
		m_model->SetPosition( m_transform.position );
		m_model->Update( deltaTime );
	}

	m_pathToTarget = cGameManager::GetInstance()->GetGameBoard()->FindPathAstar( m_transform.position, m_targetPos );
}

void cPlayerUnit::Draw()
{
	cGameObject::Draw();

	for ( int i = 1; i < (int) m_pathToTarget.size(); i++ )
	{
		cGameRenderer::GetInstance()->DrawImmediate( m_pathToTarget[i-1], 0xff0000ff );
		cGameRenderer::GetInstance()->DrawLine( m_pathToTarget[i-1], m_pathToTarget[i], 0xff0000ff );
	}

	if (m_pathToTarget.size() > 1)
	{
		cGameRenderer::GetInstance()->DrawImmediate( m_pathToTarget[0], 0xffff0000);
		cGameRenderer::GetInstance()->DrawImmediate( m_pathToTarget[m_pathToTarget.size()-1], 0xffff0000);
	}

	if( m_model )
		m_model->Draw();
}
