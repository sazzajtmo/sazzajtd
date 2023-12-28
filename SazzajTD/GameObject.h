#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "Utilities.h"
#include "GameDefs.h"
#include <memory>

//base class for Game Objects
//game objects can be enemy units, ally buildings, static objects, etc.

class cAnimatedTexture;

class cGameObject
{
	cGameObject();
public:
	cGameObject(eGameObjectTypes type);
	virtual ~cGameObject();

	virtual void						Init();
	virtual	void						Cleanup();
	virtual void						Update( float deltaTime );
	virtual void						Draw();
	virtual	void						DrawDebug();

	virtual	void						SetPosition( const tVector2Df& position );

			[[nodiscard]]
			tVector2Df					GetPosition() const;

			[[nodiscard]]
			eGameObjectTypes			GetType() const;

protected:
	std::unique_ptr<cAnimatedTexture>	m_model;
	tGameTransform						m_transform;
	eGameObjectTypes					m_type{ eGameObjectTypes::None };
};

#endif