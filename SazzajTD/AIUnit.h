#ifndef AI_UNIT_H
#define AI_UNIT_H

#include "GameObject.h"
#include "GameDefs.h"
#include <vector>

class cAnimatedTexture;

class cAIUnit : public cGameObject
{
public:
	cAIUnit();
	~cAIUnit();

	void					Init() override;
	void					Update( float deltaTime ) override;
	void					Draw() override;

protected:
	tGameTransform			m_targetPos;
	float					m_speed = 100.f;

	std::vector<tVector2Df>	m_pathToTarget;
	int						m_currPathPointIdx	= -1;
};

#endif