#include "GameObject.h"
#include <math.h>

#include "GameRenderer.h"
#include "AnimatedTexture.h"

cGameObject::cGameObject()
{
}

cGameObject::~cGameObject()
{
}

void cGameObject::Init()
{
}

void cGameObject::Cleanup()
{
}

void cGameObject::Update(float deltaTime)
{
	if (m_model)
	{
		m_model->Update(deltaTime);
		float modelW, modelH;
		m_model->GetFrameDims(modelW, modelH);
		m_model->SetPosition(m_transform.position - tVector2Df(modelW * 0.5f, modelH * 0.5f));
	}
}

void cGameObject::Draw()
{
	if (m_model)
		m_model->Draw();
}

void cGameObject::DrawDebug()
{
}

void cGameObject::SetPosition(const tVector2Df& position)
{
	m_transform.position = position;
}
