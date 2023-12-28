#ifndef GUN_BUILDING_UNIT_H
#define GUN_BUILDING_UNIT_H

#include "GameObject.h"
#include "Utilities.h"
#include <vector>

class cAnimatedTexture;

class cGunBuildingUnit : public cGameObject
{
public:
	cGunBuildingUnit();
	~cGunBuildingUnit();

	void					Init() override;
	void					Update( float deltaTime ) override;
	void					Draw() override;

protected:
};

#endif