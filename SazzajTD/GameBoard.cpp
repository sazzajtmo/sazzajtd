#include "GameBoard.h"
#include <cmath>
#include <algorithm>
#include <queue>
#include "GameRenderer.h"
#include "MemHelper.h"
#include "GameBoardGenerator.h"

cGameBoard::cGameBoard()
{
}

cGameBoard::~cGameBoard()
{
}

void cGameBoard::Cleanup()
{
	for( auto& gridPoint : m_boardGrid )
		delete gridPoint;

	m_boardGrid.clear();

	cGameObject::Cleanup();
}

void cGameBoard::InitPathfinding( const std::string& walkableMapTextureFilePath )
{
	if (!cGameRenderer::GetInstance()->LoadCustomSurface( walkableMapTextureFilePath ))
	{
		return;
	}

	const int divs = 60; //100x100 board
	int boardWidth = 0, boardHeight = 0;

	cGameRenderer::GetInstance()->GetSurfaceSize( walkableMapTextureFilePath, boardWidth, boardHeight );
	const int widthStep		= boardWidth / divs;
	const int heightStep	= boardHeight / divs;
	const float threshold	= 0.3f;

	auto linkNeighbours = [this](tPoint* gridPoint, int x, int y)
	{
		tPoint* neighbor = FindGridPoint( static_cast<float>(x), static_cast<float>(y), 1.f ); //high acurracy find, because we actually know where the grid point is. tolerance should be FLT_EPSILON

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
			tColor color = cGameRenderer::GetInstance()->GetRGBA( walkableMapTextureFilePath, x, y );
			
			if (color.r > threshold)
			{
				tPoint* newGridPoint = snew tPoint();
				newGridPoint->pos.x = ( (float) x / (float) boardWidth )	* (float) boardWidth;
				newGridPoint->pos.y = ( (float) y / (float) boardHeight )	* (float) boardHeight;
				newGridPoint->cost	= 0.f;//std::rand() % 100;//( ( color.r - threshold ) / ( 1.f - threshold ) ) * 10.f;
	
				linkNeighbours( newGridPoint, x - widthStep	, y					);	//W
				linkNeighbours( newGridPoint, x - widthStep	, y - heightStep	);	//NW
				linkNeighbours( newGridPoint, x				, y - heightStep	);	//N
				linkNeighbours( newGridPoint, x + widthStep	, y - heightStep	);	//NE

				m_boardGrid.push_back( newGridPoint );	
			}
		}
	}
}

cGameBoard::tPoint* cGameBoard::FindGridPoint(float x, float y, float tolerance) const
{
	const int64_t n = static_cast<int64_t>(m_boardGrid.size());

	if( n == 0u )
		return nullptr;
	
	int64_t l = 0;
	int64_t r = n-1;
	int64_t m = 0;
	bool found = false;

	while (l <= r)
	{
		m = l + ( r - l ) / 2;

		if (std::abs( y - m_boardGrid[m]->pos.y ) < tolerance)
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

	while( l > 0 && std::abs( y - m_boardGrid[l-1]->pos.y ) < tolerance ) l--;
	while( r < n-1 && std::abs( y - m_boardGrid[r+1]->pos.y ) < tolerance ) r++;

	found	= false;

	while (l <= r)
	{
		m = l + ( r - l ) / 2;

		if (std::abs(m_boardGrid[m]->pos.x - x) < tolerance)
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

std::vector<tVector2Df> cGameBoard::FindPathBFS(const tVector2Df& startPos, const tVector2Df& endPos) const
{
	tPoint* start		= FindGridPoint( startPos.x, startPos.y );
	const tPoint* end	= FindGridPoint( endPos.x, endPos.y );

	if( !start || !end )
		return {};

	std::queue<tPoint*>			frontier;
	std::map<tPoint*, tPoint*>	visitMap;	//next visited from current

	frontier.push( start );
	visitMap[start] = nullptr;

	tPoint* current = nullptr;

	while (!frontier.empty())
	{
		current = frontier.front();
		frontier.pop();

		if( current == end )
			break;

		for (const auto& neighbour : current->neighbours)
		{
			if (visitMap.count(neighbour) == 0u)
			{
				frontier.push( neighbour );
				visitMap[neighbour] = current;
			}
		}
	}

	if( !current )
		return {};

	std::vector<tVector2Df> path;

	while (current != start)
	{
		path.push_back( current->pos );
		current = visitMap[current];
	}

	path.push_back( start->pos );

	return path;
}

std::vector<tVector2Df> cGameBoard::FindPathAstar(const tVector2Df& startPos, const tVector2Df& endPos) const
{
	tPoint* start	= FindGridPoint( startPos.x, startPos.y );
	const tPoint* end		= FindGridPoint( endPos.x, endPos.y );

	if( !start || !end )
		return {};	

	auto prioQueComp = [](const std::pair<tPoint*, float>& a, const std::pair<tPoint*, float>& b)
	{
		return a.second > b.second;
	};

	std::priority_queue <std::pair<tPoint*, float>, std::vector<std::pair<tPoint*, float>>, decltype(prioQueComp)> 
									frontier(prioQueComp);
	std::map<tPoint*, tPoint*>		visitMap;
	std::map<tPoint*, float>		costMap;

	frontier.push( std::pair<tPoint*, float>( start, 0.f ) );
	visitMap[start] = nullptr;
	costMap[start]	= 0.f;

	std::pair<tPoint*, float> current;

	auto heuristic = []( const tPoint* current, const tPoint* goal )
	{
		return abs(current->pos.x - goal->pos.x) + abs(current->pos.y - goal->pos.y);
	};

	while (!frontier.empty())
	{
		current = frontier.top();
		frontier.pop();

		if( current.first == end )
			break;

		for (const auto& neighbour : current.first->neighbours)
		{
			float newCostSoFar = costMap[current.first] + neighbour->cost;

			if (costMap.count(neighbour) == 0u || newCostSoFar < costMap[neighbour])
			{
				costMap[neighbour] = newCostSoFar;
				frontier.push( std::pair<tPoint*, float>( neighbour, heuristic( neighbour, end ) ) );
				visitMap[neighbour] = current.first;
			}
		}
	}

	if( !current.first || current.first != end )
		return {};

	tPoint* trav = current.first;

	std::vector<tVector2Df> path;

	while (trav != start)
	{
		path.push_back( trav->pos );
		trav = visitMap[trav];
	}

	path.push_back( start->pos );
	std::reverse( path.begin(), path.end() );

	return path;
}

void cGameBoard::Update(float deltaTime)
{
	//m_currPath	= FindPathBFS( tVector2Df( 315.f, 80.f ), tVector2Df( 525.f, 64.f ) );
	//m_currPath2	= FindPathAstar( tVector2Df( 315.f, 80.f ), tVector2Df( 525.f, 64.f ) );
}

void cGameBoard::Draw()
{
	for (const auto* gridPoint : m_boardGrid)
	{
		for (const auto* neighbour : gridPoint->neighbours)
		{
			cGameRenderer::GetInstance()->DrawLine( gridPoint->pos, neighbour->pos, 0x3fffffff );
		}

		cGameRenderer::GetInstance()->DrawImmediate( gridPoint->pos, 0x3fffffff );
	}

	for ( size_t i = 1u; i < m_currPath.size(); i++ )
	{
		cGameRenderer::GetInstance()->DrawImmediate( m_currPath[i-1], 0xff0000ff );
		cGameRenderer::GetInstance()->DrawLine( m_currPath[i-1], m_currPath[i], 0xff0000ff );
	}

	for ( size_t i = 1u; i < m_currPath2.size(); i++ )
	{
		cGameRenderer::GetInstance()->DrawImmediate( m_currPath2[i-1], 0xff00ff00 );
		cGameRenderer::GetInstance()->DrawLine( m_currPath2[i-1], m_currPath2[i], 0xff00ff00 );
	}

	cGameRenderer::GetInstance()->DrawImmediate( tVector2Df( 315.f, 80.f ), 0xffff0000 );
	cGameRenderer::GetInstance()->DrawImmediate( tVector2Df( 525.f, 64.f ), 0xffff0000 );
}
