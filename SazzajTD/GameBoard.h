#ifndef GAME_BOARD_H
#define GAME_BOARD_H

#include "GameObject.h"
#include "GameDefs.h"
#include <vector>

class cGameBoard : public cGameObject
{
public:
	struct tPoint
	{
		tVector2Df				pos;
		float					cost = 0.f;
		std::vector<tPoint*>	neighbours;
	};

public:
	cGameBoard();
	virtual ~cGameBoard();
			
		void					Cleanup() override;

		void					InitPathfinding( int boardWidth, int boardHeight );
		tPoint*					FindGridPoint( float x, float y, float tolerance = 10.f ) const;

		std::vector<tVector2Df>	FindPathBFS( const tVector2Df& start, const tVector2Df& end ) const;
		std::vector<tVector2Df>	FindPathAstar( const tVector2Df& start, const tVector2Df& end ) const;

		void					Update( float deltaTime ) override;
		void					Draw() override;

private:
		std::vector<tPoint*>	m_boardGrid;
		tPoint					m_nullPoint;

		std::vector<tVector2Df>	m_currPath;
		std::vector<tVector2Df>	m_currPath2;
		
};

#endif