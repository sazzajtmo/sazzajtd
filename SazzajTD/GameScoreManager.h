#ifndef GAME_SCORE_MANAGER_H
#define GAME_SCORE_MANAGER_H

#include "GameDefs.h"

//dedicated class could do the following:
//- update continuous score rather than event based
//- log
//- save/load to a leaderboard
class cGameScoreManager final
{
public:
	cGameScoreManager();
	~cGameScoreManager();

			void		Update(float deltaTime);
			void		Reset();

	inline	float		GetScore() const { return m_score; }
			void		AddScore( eGameScoreSource source );
private:
			float		m_score					= 0.f;
			float		m_lifetimeScoreTimer	= 0.f;
	
};

#endif