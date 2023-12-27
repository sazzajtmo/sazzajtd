#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "GameDefs.h"
#include <memory>

//base class for Game Objects
//game objects can be enemy units, ally buildings, static objects, etc.

class cAnimatedTexture;

class cGameObject
{
public:
	cGameObject();
	virtual ~cGameObject();

	virtual void						Init();
	virtual	void						Cleanup();
	virtual void						Update( float deltaTime );
	virtual void						Draw();
	virtual	void						DrawDebug();

	virtual	void						SetPosition( const tVector2Df& position );

protected:
	std::unique_ptr<cAnimatedTexture>	m_model;
	tGameTransform						m_transform;
};

#endif