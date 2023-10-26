#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "GameDefs.h"

class cGameObject
{
public:
	cGameObject();
	~cGameObject();

	void					Update( float deltaTime );
	void					Draw();

protected:
	int						m_health = 0;
	tGameTransform			m_transform;
};

#endif