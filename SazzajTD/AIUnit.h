#ifndef AI_UNIT_H
#define AI_UNIT_H

#include "GameObject.h"
#include "Utilities.h"
#include <vector>

class cAnimatedTexture;

class cAIUnit : public cGameObject, public std::enable_shared_from_this<cAIUnit>
{
public:
	cAIUnit();
	~cAIUnit();

			void						Init() override;
			void						Update( float deltaTime ) override;
			void						Draw() override;

	inline	void						SetSpeedFactor(float factor)	{ m_speedFactor = factor; }
	inline	float						GetSpeedFactor() const			{ return m_speedFactor;  }

			void						ReceiveDamage(float dmgValue);
			void						OnDeath();
			void						OnReachingExitPoint();


protected:
			tGameTransform				m_targetPos;

			float						m_speed				= GameConfig::values.enemy_speed;
			float						m_speedFactor		= 1.f;

			float						m_health			= GameConfig::values.enemy_hp;

			std::vector<tVector2Df>		m_pathToTarget;
			int							m_currPathPointIdx	= -1;
};

#endif