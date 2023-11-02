#ifndef GAME_BOARD_H
#define GAME_BOARD_H

#include "GameObject.h"
#include "GameDefs.h"
#include <vector>
#include <string>

class cGameBoard : public cGameObject
{
public:
	struct tPoint
	{
		tVector2Df				pos;
		float					cost		= 0.f;
		float					visitCost	= 0.f;
		std::vector<tPoint*>	neighbours;
	};

public:
	cGameBoard();
	virtual ~cGameBoard();
			
			void					Cleanup() override;

			void					Init() override;
			void					InitPathfinding( const std::string& walkableMapTextureFilePath );
			tPoint*					FindGridPoint( float x, float y, float tolerance = 10.f ) const;

			std::vector<tVector2Df>	FindPathBFS( const tVector2Df& start, const tVector2Df& end ) const;
			std::vector<tVector2Df>	FindPathAstar( const tVector2Df& start, const tVector2Df& end ) const;

			void					Update( float deltaTime ) override;
			void					Draw() override;

	inline	const tVector2Df&		GetEntryPoint() const	{ return m_entryPoint; }
	inline	const tVector2Df&		GetExitPoint() const	{ return m_exitPoint; }

private:
		std::vector<tPoint*>	m_boardGrid;
		tPoint					m_nullPoint;

		tVector2Df				m_entryPoint;
		tVector2Df				m_exitPoint;
		
};

#endif