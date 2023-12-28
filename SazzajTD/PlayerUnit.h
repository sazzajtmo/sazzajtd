#ifndef PLAYER_UNIT_H
#define PLAYER_UNIT_H

#include "GameObject.h"
#include "Utilities.h"
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
	void					DrawDebug() override;

protected:
	tVector2Df				m_targetPos;
	std::vector<tVector2Df>	m_pathToTarget;
	int						m_currPathPointIdx	= -1;

	int						m_mouseCbHandle		= -1;
	
};

#endif