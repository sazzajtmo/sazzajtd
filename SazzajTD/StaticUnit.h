#ifndef STATIC_UNIT_H
#define STATIC_UNIT_H

#include "GameObject.h"

class cAnimatedTexture;

class cStaticUnit : public cGameObject, public std::enable_shared_from_this<cStaticUnit>
{
public:
	cStaticUnit();
	~cStaticUnit();

			void					Init() override;
			void					Update( float deltaTime ) override;
			void					Draw() override;

	inline	void					SetLifetime(float lifetime) { m_lifetime = lifetime;  }
	inline	float					GetLifetime()				{ return m_lifetime; }

private:
			float					m_lifetime = FLT_MAX;
	
};

#endif