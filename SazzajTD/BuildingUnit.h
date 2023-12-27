#ifndef BUILDING_UNIT_H
#define BUILDING_UNIT_H

#include "GameObject.h"
#include "GameDefs.h"
#include <vector>

class cAnimatedTexture;

class cBuildingUnit : public cGameObject
{
public:
	cBuildingUnit();
	~cBuildingUnit();

	void					Init() override;
	void					Update( float deltaTime ) override;
	void					Draw() override;

protected:
};

#endif