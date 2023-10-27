#include "AIUnit.h"
#include <math.h>

#include "GameRenderer.h"

cAIUnit::cAIUnit()
{
	m_transform.position.x = 300.f;
	m_transform.position.y = 300.f;

	m_targetPos.position = m_transform.position + tVector2Df( ( static_cast<float>(std::rand() % 200) - 100.f ) / 2.f, ( static_cast<float>(std::rand() % 200) - 100.f ) / 2.f );
		m_speed = static_cast<float>( std::rand() % 100 );
}

cAIUnit::~cAIUnit()
{
}

void cAIUnit::Update(float deltaTime)
{
	if (std::abs(distance(m_transform.position, m_targetPos.position)) < 1.1f)
	{
		m_targetPos.position = m_transform.position + tVector2Df( ( static_cast<float>(std::rand() % 200) - 100.f ) / 2.f, ( static_cast<float>(std::rand() % 200) - 100.f ) / 2.f );
		m_speed = static_cast<float>( std::rand() % 100 );
	}

	tVector2Df dirToDest = directionNormalized( m_transform.position, m_targetPos.position );

	m_transform.position += dirToDest * m_speed * deltaTime;
}

void cAIUnit::Draw()
{
	cGameRenderer::GetInstance()->DrawImmediate( m_targetPos );

	cGameObject::Draw();
}
