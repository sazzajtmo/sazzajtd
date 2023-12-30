#include "StaticUnit.h"
#include "GameManager.h"


cStaticUnit::cStaticUnit()
: cGameObject( eGameObjectTypes::Static )
{
	m_transform.position.x = -300.f;
	m_transform.position.y = -300.f;
}

cStaticUnit::~cStaticUnit()
{
}

void cStaticUnit::Init()
{
}

void cStaticUnit::Update(float deltaTime)
{
	cGameObject::Update(deltaTime);

	m_lifetime -= deltaTime;

	if (m_lifetime < 0.f)
	{
		cGameManager::GetInstance()->DespawnObject(shared_from_this());
	}
}

void cStaticUnit::Draw()
{
	cGameObject::Draw();
}
