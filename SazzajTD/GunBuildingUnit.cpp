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

	std::vector<std::shared_ptr<cGameObject>> proximityObjects;

	cGameManager::GetInstance()->GetGameObjectsInRadius(eGameObjectTypes::Enemy, proximityObjects, m_transform.position, GameConfig::values.building_slow_radius);

	for (const auto& object : proximityObjects)
	{
		//dynamic_cast<cAIUnit*>(object.get())->SetSpeedFactor(GameConfig::values.building_slow_factor);
		cGameRenderer::GetInstance()->DrawLine(object->GetPosition(), m_transform.position, 0xffff0000);
	}
}

void cGunBuildingUnit::Draw()
{
	cGameObject::Draw();
}
