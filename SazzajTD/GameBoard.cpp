#include "GameBoard.h"
#include <cmath>
#include <algorithm>
#include <queue>
#include <chrono>
#include <ctime>
#include "GameRenderer.h"
#include "MemHelper.h"
#include "GameBoardGenerator.h"
#include "GameLog.h"

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

void cGameBoard::Init()
{
	Cleanup();

	using namespace std::chrono;
	std::time_t currentTime_t = system_clock::to_time_t(system_clock::now());
	m_boardName = "board_" + std::to_string(currentTime_t);

	std::string boardNameTextureFileName			= m_boardName + ".png";
	std::string boardNameWalkableTextureFileName	= m_boardName + "_WalkMask.png";

	const int tileSize	= 24;
	const int boardRows	= 20;
	const int boardCols = 27;
	const int junctions	= 3 + std::rand() % 7;

	auto grid = CreateGameBoard( tileSize, boardRows, boardCols, junctions, m_entryPoint, m_exitPoint );

	cGameRenderer::GetInstance()->ExportGridToFile(grid, tileSize, m_boardName);
	cGameRenderer::GetInstance()->SetBackground(boardNameTextureFileName);
	
	InitPathfinding(grid, tileSize);
	//InitPathfinding(boardNameWalkableTextureFileName);
}

void cGameBoard::InitPathfinding( const std::string& walkableMapTextureFilePath )
{
	if (!cGameRenderer::GetInstance()->LoadCustomSurface( walkableMapTextureFilePath ))
	{
		GAME_LOG("GameBoard::InitPathfinding >> Cannot open walkable mask surface.");
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

	const float maxCost = 500.f;
	//edges cost correction
	for (auto& gridPoint : m_boardGrid)
	{
		float newCost = maxCost * ( ( 8.f - 1.f * gridPoint->neighbours.size() ) / 8.f );
		gridPoint->cost = newCost;
	}
}

void cGameBoard::InitPathfinding( const std::vector<std::vector<int8_t>>& grid, int tileSize )
{
	if (grid.size() == 0u)
		return;

	auto linkNeighbours = [this]( tPoint* gridPoint, int x, int y, const std::vector<std::vector<int>>& neighboursIndices, const std::vector<std::vector<int8_t>>& grid )
	{
		if (y < 0 || x >= neighboursIndices[0].size() || grid[y][x] == 0u)
			return;

		int boardCellIdx = neighboursIndices[y][x];
		
		if (boardCellIdx == -1)
			return;

		tPoint* neighbor = m_boardGrid[boardCellIdx];

		if (neighbor)
		{
			neighbor->neighbours.push_back(gridPoint);
			gridPoint->neighbours.push_back(neighbor);
		}
	};

	const int	rows		= (int) grid.size();
	const int	cols		= (int) grid[0].size();

	std::vector<std::vector<int>> neighbours( rows, std::vector<int>(cols, -1) );

	for( int y = 0; y < rows; y++ )
	{
		for( int x = 0; x < cols; x++ )
		{
			if (grid[y][x] == 0u 
				&& ( y == 0 || grid[y-1][x] == 0u )
				&& ( x == 0 || grid[y][x-1] == 0u ))
				continue;

			tPoint* newGridPoint = snew tPoint();
			newGridPoint->pos.x = static_cast<float>( tileSize * x );
			newGridPoint->pos.y = static_cast<float>( tileSize * y );
			newGridPoint->cost = 0.f;//std::rand() % 100;//( ( color.r - threshold ) / ( 1.f - threshold ) ) * 10.f;

			linkNeighbours(newGridPoint, x - 1	, y		, neighbours, grid);	//W
			linkNeighbours(newGridPoint, x - 1	, y - 1	, neighbours, grid);	//NW
			linkNeighbours(newGridPoint, x		, y - 1	, neighbours, grid);	//N

			if( y > 0 && grid[y-1][x] != 0u )
				linkNeighbours(newGridPoint, x + 1	, y - 1	, neighbours, grid);	//NE

			m_boardGrid.push_back(newGridPoint);
			neighbours[y][x] = static_cast<int>(m_boardGrid.size() - 1);
		}
	}

	const float maxCost = 500.f;
	//edges cost correction
	for (auto& gridPoint : m_boardGrid)
	{
		float newCost = maxCost * ((8.f - 1.f * gridPoint->neighbours.size()) / 8.f);
		gridPoint->cost = newCost;
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
			y = m_boardGrid[m]->pos.y;
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

	while( l > 0 && std::abs( y - m_boardGrid[l-1]->pos.y ) <= FLT_EPSILON ) l--;
	while( r < n-1 && std::abs( y - m_boardGrid[r+1]->pos.y ) <= FLT_EPSILON ) r++;

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
	tPoint*			start	= FindGridPoint( startPos.x, startPos.y );
	const tPoint*	end		= FindGridPoint( endPos.x, endPos.y );

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

		//const auto [point, prio] = current;

		if( current.first == end )
			break;

		float costSoFar = costMap[current.first];

		for (const auto& neighbour : current.first->neighbours)
		{
			float newCostSoFar = costSoFar + neighbour->cost + neighbour->visitCost;
						
			if (costMap.count(neighbour) == 0u || newCostSoFar < costMap[neighbour])
			{
				costMap[neighbour] = newCostSoFar;
				frontier.push( std::pair<tPoint*, float>( neighbour, newCostSoFar + heuristic( neighbour, end ) ) );
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
		trav->visitCost += 15.f;
		path.push_back( trav->pos );
		trav = visitMap[trav];
	}

	path.push_back( start->pos );
	std::reverse( path.begin(), path.end() );

	return path;
}

void cGameBoard::Update(float deltaTime)
{
}

void cGameBoard::Draw()
{
#define DEBUG_PATHFINDING
#ifdef DEBUG_PATHFINDING
	for (const auto* gridPoint : m_boardGrid)
	{
		for (const auto* neighbour : gridPoint->neighbours)
		{
			cGameRenderer::GetInstance()->DrawLine( gridPoint->pos, neighbour->pos, 0x3fffffff );
		}

		cGameRenderer::GetInstance()->DrawImmediate( gridPoint->pos, 0x3fffffff );
	}
#endif
}

std::vector<std::vector<int8_t>> cGameBoard::CreateGameBoard(int tileSize, int rows, int cols, int numJunctions, tVector2Df& entryPointF, tVector2Df& exitPointF)
{
	std::vector<std::vector<int8_t>> grid(rows, std::vector<int8_t>(cols));

	for (auto& gridLine : grid)
		memset(gridLine.data(), 0, gridLine.size());

	std::pair<int, int> entryPoint = { std::rand() % rows, 0 };

	std::vector<std::pair<int, int>> junctions;

	for (int i = 0; i < numJunctions; i++)
	{
		junctions.push_back({ (std::rand() % (rows - 2)) + 1, (std::rand() % (cols - 2)) + 1 });
	}

	std::sort(junctions.begin(), junctions.end(), [entryPoint](const std::pair<int, int>& lh, const std::pair<int, int>& rh)
		{
			return (abs(entryPoint.first - lh.first) + abs(entryPoint.second - lh.second)) < (abs(entryPoint.first - rh.first) + abs(entryPoint.second - rh.second));
		});

	junctions.insert(junctions.begin(), entryPoint);

	//exit point is last junction position projected on the right edge of the board
	std::pair<int, int> exitPoint = junctions.back();
	exitPoint.second = cols - 1;
	junctions.push_back(exitPoint);

	grid[entryPoint.first][entryPoint.second] = 1;
	grid[exitPoint.first][exitPoint.second] = 1;

	auto linkJunctions = [&grid](const std::pair<int, int>& linkFrom, const std::pair<int, int>& linkTo)
		{
			int yJunc = linkFrom.first;

			while (yJunc != linkTo.first)
			{
				grid[yJunc][linkFrom.second] = 1;
				yJunc += yJunc > linkTo.first ? -1 : 1;
			}

			int xJunc = linkFrom.second;

			while (xJunc != linkTo.second)
			{
				grid[linkTo.first][xJunc] = 1;
				xJunc += xJunc > linkTo.second ? -1 : 1;
			}
		};

	//generate a simple path through junctions connecting entry and exit points
	for (int i = 1; i < (int)junctions.size(); i++)
		linkJunctions(junctions[i], junctions[i - 1]);

	//generate link between junctions to create cycles
	for (int i = 1; i < (int)junctions.size() - 2; i++)
		linkJunctions(junctions[i], junctions[i + 1]);

	//debugging
	for (int i = 1; i < (int)junctions.size(); i++)
	{
		const auto& junction = junctions[i];

		grid[junction.first][junction.second] = 2;
	}

	float smallError = 0.f;// tileSize * 0.2f;

	//yes inverted because I'm stupid
	entryPointF.y = static_cast<float>(entryPoint.first * tileSize + smallError );
	entryPointF.x = static_cast<float>(entryPoint.second * tileSize + smallError );

	exitPointF.y = static_cast<float>(exitPoint.first * tileSize + smallError );
	exitPointF.x = static_cast<float>(exitPoint.second * tileSize + smallError );

	return grid;
}