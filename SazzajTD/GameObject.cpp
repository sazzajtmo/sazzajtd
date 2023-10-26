#include "GameObject.h"
#include <math.h>

#include "GameRenderer.h"

cGameObject::cGameObject()
{
}

cGameObject::~cGameObject()
{
}

void cGameObject::Update(float deltaTime)
{
	static float accumSin = 0.f;
	static float accumCos = 0.f;

	accumSin += 360.f * 3.14f / 180.f * deltaTime;
	accumCos += 360.f * 3.14f / 180.f * deltaTime;

	m_transform.position.y = 100.f * ( cosf( accumCos ) * 0.5f + 1.f );;
	m_transform.position.x = 100.f * ( sinf( accumSin ) * 0.5f + 1.f );
}

void cGameObject::Draw()
{
	cGameRenderer::GetInstance()->DrawImmediate( m_transform );
}
