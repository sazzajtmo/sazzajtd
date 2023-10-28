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
			
		void					InitPathfinding( int boardWidth, int boardHeight );
		tPoint*					FindGridPoint( float x, float y );

		void					Update( float deltaTime ) override;
		void					Draw() override;

private:
		std::vector<tPoint*>	m_boardGrid;
		tPoint					m_nullPoint;
		
};

#endif