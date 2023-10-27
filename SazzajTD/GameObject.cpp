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
}

void cGameObject::Draw()
{
	cGameRenderer::GetInstance()->DrawImmediate( m_transform );
}
