#include "GameScoreManager.h"
#include "GameLog.h"

cGameScoreManager::cGameScoreManager()
{
}

cGameScoreManager::~cGameScoreManager()
{
}

void cGameScoreManager::Update(float deltaTime)
{
	m_lifetimeScoreTimer -= deltaTime;

	if (m_lifetimeScoreTimer <= 0.f)
	{
		m_lifetimeScoreTimer += 1.f;
		AddScore( eGameScoreSource::Time );
	}
}

void cGameScoreManager::Reset()
{
	m_lifetimeScoreTimer	= 0.f;
	m_score					= 0.f;
}

void cGameScoreManager::AddScore(eGameScoreSource source)
{
	m_score += GameConfig::scoring[static_cast<int>(source)];

	GAME_LOG( "%0.2f score points added from '%s' to a total of %0.2f", GameConfig::scoring[static_cast<int>(source)], GameConfig::scoreSourceNames[static_cast<int>(source)].c_str(), m_score);
}




