#include "GameBoard.h"
#include <cmath>
#include <algorithm>
#include "GameRenderer.h"

cGameBoard::cGameBoard()
{
}

cGameBoard::~cGameBoard()
{
}

void cGameBoard::InitPathfinding( int maxBoardWidth, int maxBoardHeight )
{
	const std::string mapTextureFileName = "boardMap.png";//"World_elevation_map.png";//

	if (!cGameRenderer::GetInstance()->LoadCustomSurface( mapTextureFileName ))
	{
		return;
	}

	const int divs = 30; //100x100 board
	int boardWidth = 0, boardHeight = 0;

	cGameRenderer::GetInstance()->GetSurfaceSize( mapTextureFileName, boardWidth, boardHeight );
	const int widthStep		= boardWidth / divs;
	const int heightStep	= boardHeight / divs;
	const float threshold	= 0.3f;

	auto linkNeighbours = [this](tPoint* gridPoint, int x, int y)
	{
		tPoint* neighbor = FindGridPoint( x, y );

		if( neighbor ) 
		{
			neighbor->neighbours.push_back( gridPoint );
			gridPoint->neighbours.push_back( neighbor );
		}
	};
	
	for (int y = 0; y < boardHeight; y += heightStep)
	{
		for (int x = 0; x < boardWidth; x += widthStep)
		{
			tColor color = cGameRenderer::GetInstance()->GetRGBA( mapTextureFileName, x, y );
			
			if (color.r > threshold)
			{
				tPoint* newGridPoint = new tPoint();
				newGridPoint->pos.x = ( (float) x / (float) boardWidth )	* (float) maxBoardWidth;
				newGridPoint->pos.y = ( (float) y / (float) boardHeight )	* (float) maxBoardHeight;				
	
				linkNeighbours( newGridPoint, x - widthStep	, y					);	//W
				linkNeighbours( newGridPoint, x - widthStep	, y - heightStep	);	//NW
				linkNeighbours( newGridPoint, x				, y - heightStep	);	//N
				linkNeighbours( newGridPoint, x + widthStep	, y - heightStep	);	//NE

				m_boardGrid.push_back( newGridPoint );	
			}
		}
	}
}

cGameBoard::tPoint* cGameBoard::FindGridPoint(float x, float y)
{
	const int n = m_boardGrid.size();

	if( n == 0u )
		return nullptr;

	tVector2Df toFind( x, y );
	
	int l = 0;
	int r = n-1;
	int m = 0;
	bool found = false;

	while (l <= r)
	{
		m = l + ( r - l ) / 2;

		if (std::abs( y - m_boardGrid[m]->pos.y ) < 1.f)
		{
			found = true;
			break;
		}
		else if (y > m_boardGrid[m]->pos.y)
		{
			l = m + 1;
		}
		else
		{
			r = m - 1;
		}
	}

	if( !found )
		return nullptr;

	l = m;
	r = m;

	while( l > 0 && std::abs( y - m_boardGrid[l-1]->pos.y ) < 1.f ) l--;
	while( r < n-1 && std::abs( y - m_boardGrid[r+1]->pos.y ) < 1.f ) r++;

	found	= false;

	while (l <= r)
	{
		m = l + ( r - l ) / 2;

		if (std::abs(m_boardGrid[m]->pos.x - x) < 1.f)
		{
			found = true;
			break;
		}
		else if (x > m_boardGrid[m]->pos.x)
		{
			l = m + 1;
		}
		else
		{
			r = m - 1;
		}
	}

	return found ? m_boardGrid[m] : nullptr;
}

void cGameBoard::Update(float deltaTime)
{
}

void cGameBoard::Draw()
{
	for (const auto& gridPoint : m_boardGrid)
	{
		for (const auto& neighbour : gridPoint->neighbours)
		{
			cGameRenderer::GetInstance()->DrawLine( gridPoint->pos, neighbour->pos, 0xffff0000 );
		}

		cGameRenderer::GetInstance()->DrawImmediate( gridPoint->pos, 0xffffff00 );
	}

	//cGameRenderer::GetInstance()->DrawImmediate( m_transform );
}
