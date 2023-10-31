#ifndef PLAYER_UNIT_H
#define PLAYER_UNIT_H

#include "GameObject.h"
#include "GameDefs.h"
#include <vector>

class cAnimatedTexture;

class cPlayerUnit : public cGameObject
{
public:
	cPlayerUnit();
	~cPlayerUnit();

	void					Init() override;
	void					Update( float deltaTime ) override;
	void					Draw() override;

protected:
	tVector2Df				m_targetPos;
	std::vector<tVector2Df>	m_pathToTarget;

	int						m_mouseCbHandle = -1;

	cAnimatedTexture*		m_model			= nullptr;
	
};

#endif