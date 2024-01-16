#include "BuildingUnit.h"

#include "GameRenderer.h"
#include "AnimatedTexture.h"
#include "GameManager.h"
#include "GameBoard.h"
#include "GameLog.h"
#include "AIUnit.h"

const float BUILDING_RADIUS = 60.f;

cBuildingUnit::cBuildingUnit()
: cGameObject( eGameObjectTypes::Building )
{
	m_transform.position.x = -300.f;
	m_transform.position.y = -300.f;
}

cBuildingUnit::~cBuildingUnit()
{
}

void cBuildingUnit::Init()
{
	m_model = cAnimatedTexture::Load("buildings/building_slow.png");
}

void cBuildingUnit::Update(float deltaTime)
{
	cGameObject::Update(deltaTime);

	std::vector<std::shared_ptr<cGameObject>> proximityObjects;

	cGameManager::GetInstance()->GetGameObjectsInRadius(eGameObjectTypes::Enemy, proximityObjects, m_transform.position, GameConfig::values.building_slow_radius);

	for (const auto& object : proximityObjects)
	{
		dynamic_cast<cAIUnit*>(object.get())->SetSpeedFactor(GameConfig::values.building_slow_factor);
		cGameRenderer::GetInstance()->DrawLine(object->GetPosition(), m_transform.position, 0xff00ffff);
	}
}

void cBuildingUnit::Draw()
{
	cGameObject::Draw();
}
