#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "GameDefs.h"

//base class for Game Objects
//game objects can be enemy units, ally buildings, static objects, etc.

class cGameObject
{
public:
	cGameObject();
	virtual ~cGameObject();

	virtual void					Init();
	virtual void					Update( float deltaTime );
	virtual void					Draw();

protected:
	int								m_health = 0;
	tGameTransform					m_transform;
};

#endif