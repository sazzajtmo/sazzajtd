#ifndef AI_UNIT_H
#define AI_UNIT_H

#include "GameObject.h"
#include "GameDefs.h"

class cAIUnit : public cGameObject
{
public:
	cAIUnit();
	~cAIUnit();

	void					Update( float deltaTime ) override;
	void					Draw() override;

protected:
	tGameTransform			m_targetPos;
	float					m_speed = 10.f;
	
};

#endif