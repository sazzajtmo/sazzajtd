#include "GunBuildingUnit.h"

#include "GameRenderer.h"
#include "AnimatedTexture.h"
#include "GameManager.h"
#include "GameBoard.h"
#include "GameLog.h"
#include "AIUnit.h"

cGunBuildingUnit::cGunBuildingUnit()
: cGameObject( eGameObjectTypes::GunBuilding )
{
}

cGunBuildingUnit::~cGunBuildingUnit()
{
}

void cGunBuildingUnit::Init()
{
	m_model = cAnimatedTexture::Load("buildings/building_gun.png");
}

void cGunBuildingUnit::Update(float deltaTime)
{
	cGameObject::Update(deltaTime);

	m_fireTimer -= deltaTime;

	if (m_fireTimer <= 0.f)
	{
		std::shared_ptr<cGameObject> closestEnemy = cGameManager::GetInstance()->GetClosestGameObject(eGameObjectTypes::Enemy, m_transform.position, GameConfig::values.building_gun_radius);
		
		if (closestEnemy)
		{
			dynamic_cast<cAIUnit*>(closestEnemy.get())->ReceiveDamage(GameConfig::values.building_gun_damage);
			m_fireTimer = 1.f / GameConfig::values.building_gun_firerate;

			cGameRenderer::GetInstance()->DrawLine(m_transform.position, closestEnemy->GetPosition(), 0x1fff0000);
		}
		else
		{
			m_fireTimer = 0.016f * 3.f;
		}
	}
}

void cGunBuildingUnit::Draw()
{
	cGameObject::Draw();
}
