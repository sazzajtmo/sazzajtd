#include "GameBoard.h"
#include <cmath>
#include <algorithm>
#include <queue>
#include <chrono>
#include <ctime>
#include "GameRenderer.h"
#include "MemHelper.h"
#include "GameLog.h"
#include "AnimatedTexture.h"
#include "GameInputManager.h"
#include "GameManager.h"
#include "BuildingUnit.h"
#include <array>
#include <ranges>
#include <unordered_set>
#include <bit>

eGameBoardType cGameBoard::s_gameBoardType{eGameBoardType::SimpleLoops};

cGameBoard::cGameBoard()
: cGameObject(eGameObjectTypes::Board)
{
}

cGameBoard::~cGameBoard()
{
}

void cGameBoard::Cleanup()
{
	cGameInputManager::GetInstance()->UnRegisterForMouseEvent( m_mouseCbId );
	m_mouseCbId = -1;

	for( auto& gridPoint : m_walkPoints )
		delete gridPoint;

	m_walkPoints.clear();

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

	switch(s_gameBoardType)
	{
		case eGameBoardType::HybridDiagonals: 
			m_grid = CreateGameBoardWithDiagonalPathing(tileSize, boardRows, boardCols, junctions, m_entryPoint, m_exitPoint);
		break;
		
		case eGameBoardType::WaveFunctionCollapse: 
			m_grid = CreateGameBoardWFC(tileSize, boardRows, boardCols, junctions, m_entryPoint, m_exitPoint);
		break;

		default:
		case eGameBoardType::SimpleLoops: 
			m_grid = CreateGameBoard(tileSize, boardRows, boardCols, junctions, m_entryPoint, m_exitPoint);
		break;
	}

	cGameRenderer::GetInstance()->ExportGridToFile(m_grid, tileSize, m_boardName);
	//cGameRenderer::GetInstance()->SetBackground(boardNameTextureFileName);
	
	InitPathfinding(m_grid, tileSize);
	//InitPathfinding(boardNameWalkableTextureFileName);

	m_model				= cAnimatedTexture::Load(boardNameTextureFileName);
	
	if (m_model)
		m_model->SetPriority(0);

	m_startPointModel	= cAnimatedTexture::Load("buildings/enemy_start.png");

	if (m_startPointModel)
	{
		m_startPointModel->SetPosition(m_entryPoint);
		m_startPointModel->SetDimensions(1, 2);
		m_startPointModel->SetFramerate(4.f);
	}

	m_endPointModel		= cAnimatedTexture::Load("buildings/ally_start.png");

	if (m_endPointModel)
	{
		m_endPointModel->SetPosition(m_exitPoint);
		m_endPointModel->SetDimensions(1, 2);
		m_endPointModel->SetFramerate(4.f);
	}

	m_mouseCbId = cGameInputManager::GetInstance()->RegisterForMouseEvent([this, tileSize](const cGameInputManager::tMouseEventData& mouseEvent)
	{
			if (cGameManager::GetInstance()->GetNumAvailableBuildings() <= 0)
				return;

			//just motion data
			//if (mouseEvent.button != cGameInputManager::eMouseButton::None)
			//	return;

			const tVector2Df renderOffset = cGameRenderer::GetInstance()->GetRenderOffset();
			tVector2Df boardPosition{ mouseEvent.x - renderOffset.x, mouseEvent.y - renderOffset.y };

			m_selectedCell.x = std::lround( ( boardPosition.x - tileSize * 0.5f ) / tileSize );
			m_selectedCell.y = std::lround( ( boardPosition.y - tileSize * 0.5f ) / tileSize );

			m_selectedCellBuildable = m_selectedCell.x >= 0 && m_selectedCell.x < (int)m_grid[0].size()
									&& m_selectedCell.y >= 0 && m_selectedCell.y < (int)m_grid.size()
									&& m_grid[m_selectedCell.y][m_selectedCell.x] == static_cast<int>(eGridCellType::Buildable);

			if (m_selectedCellBuildable && mouseEvent.button == cGameInputManager::eMouseButton::Left)
			{
				m_grid[m_selectedCell.y][m_selectedCell.x] = static_cast<int>(eGridCellType::Empty);
				tGameTransform transform;
				transform.position = tVector2Df{ static_cast<float>(m_selectedCell.x * tileSize + tileSize * 0.5f), static_cast<float>(m_selectedCell.y * tileSize + tileSize * 0.5f) };
				eGameObjectTypes nextBuildingType = cGameManager::GetInstance()->GetAndUpdateCurrentBuildingType();
				cGameManager::GetInstance()->SpawnObject( nextBuildingType, transform );
			}
	});
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

				m_walkPoints.push_back( newGridPoint );	
			}
		}
	}

	const float maxCost = 500.f;
	//edges cost correction
	for (auto& gridPoint : m_walkPoints)
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
		if (y < 0 || x >= neighboursIndices[0].size() || !IsCellWalkable( grid[y][x] ) )
			return;

		int boardCellIdx = neighboursIndices[y][x];
		
		if (boardCellIdx == -1)
			return;

		tPoint* neighbor = m_walkPoints[boardCellIdx];

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
			if (!IsCellWalkable( grid[y][x] )
				&& ( y == 0 || !IsCellWalkable( grid[y-1][x] ) )
				&& ( x == 0 || !IsCellWalkable( grid[y][x-1] ) ) )
				continue;

			tPoint* newGridPoint = snew tPoint();
			newGridPoint->pos.x = static_cast<float>( tileSize * x );
			newGridPoint->pos.y = static_cast<float>( tileSize * y );
			newGridPoint->cost = 0.f;//std::rand() % 100;//( ( color.r - threshold ) / ( 1.f - threshold ) ) * 10.f;

			linkNeighbours(newGridPoint, x - 1	, y		, neighbours, grid);	//W
			linkNeighbours(newGridPoint, x - 1	, y - 1	, neighbours, grid);	//NW
			linkNeighbours(newGridPoint, x		, y - 1	, neighbours, grid);	//N

			if( y > 0 && IsCellWalkable( grid[y-1][x] ) )
				linkNeighbours(newGridPoint, x + 1	, y - 1	, neighbours, grid);	//NE

			m_walkPoints.push_back(newGridPoint);
			neighbours[y][x] = static_cast<int>(m_walkPoints.size() - 1);
		}
	}

	const float maxCost = 500.f;
	//edges cost correction
	for (auto& gridPoint : m_walkPoints)
	{
		float newCost = maxCost * ((8.f - 1.f * gridPoint->neighbours.size()) / 8.f);
		gridPoint->cost = newCost;
	}
}

cGameBoard::tPoint* cGameBoard::FindGridPoint(float x, float y, float tolerance) const
{
	const int64_t n = static_cast<int64_t>(m_walkPoints.size());

	if( n == 0u )
		return nullptr;
	
	int64_t l = 0;
	int64_t r = n-1;
	int64_t m = 0;
	bool found = false;

	while (l <= r)
	{
		m = l + ( r - l ) / 2;

		if (std::abs( y - m_walkPoints[m]->pos.y ) < tolerance)
		{
			y = m_walkPoints[m]->pos.y;
			found = true;
			break;
		}
		else if (y > m_walkPoints[m]->pos.y)
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

	while( l > 0 && std::abs( y - m_walkPoints[l-1]->pos.y ) <= FLT_EPSILON ) l--;
	while( r < n-1 && std::abs( y - m_walkPoints[r+1]->pos.y ) <= FLT_EPSILON ) r++;

	found	= false;

	while (l <= r)
	{
		m = l + ( r - l ) / 2;

		if (std::abs(m_walkPoints[m]->pos.x - x) < tolerance)
		{
			found = true;
			break;
		}
		else if (x > m_walkPoints[m]->pos.x)
		{
			l = m + 1;
		}
		else
		{
			r = m - 1;
		}
	}

	return found ? m_walkPoints[m] : nullptr;
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
	if (m_startPointModel)
		m_startPointModel->Update(deltaTime);

	if (m_endPointModel)
		m_endPointModel->Update(deltaTime);
}

void cGameBoard::Draw()
{
	cGameObject::Draw();

	if (m_startPointModel)
		m_startPointModel->Draw();

	if (m_endPointModel)
		m_endPointModel->Draw();

	if (m_selectedCell.x >= 0 && m_selectedCell.y >= 0)
	{
		const float tileSize = 24.f;

		std::array<tVector2Df, 5> selectedCellVertices =
		{
			tVector2Df{ m_selectedCell.x * tileSize			, m_selectedCell.y * tileSize },
			tVector2Df{ (m_selectedCell.x + 1) * tileSize	, m_selectedCell.y * tileSize },
			tVector2Df{ (m_selectedCell.x + 1) * tileSize	, (m_selectedCell.y + 1) * tileSize },
			tVector2Df{ m_selectedCell.x * tileSize			, (m_selectedCell.y + 1) * tileSize },
			tVector2Df{ m_selectedCell.x * tileSize			, m_selectedCell.y * tileSize }
		};

		for (size_t i = 1u; i < selectedCellVertices.size(); i++)
		{
			cGameRenderer::GetInstance()->DrawLine(selectedCellVertices[i-1], selectedCellVertices[i], m_selectedCellBuildable ? 0xff00ff00 : 0xffff0000);
		}
	}
}

void cGameBoard::DrawDebug()
{
	cGameObject::DrawDebug();

//#define DEBUG_PATHFINDING
#ifdef DEBUG_PATHFINDING
	for (const auto* gridPoint : m_walkPoints)
	{
		for (const auto* neighbour : gridPoint->neighbours)
		{
			cGameRenderer::GetInstance()->DrawLine(gridPoint->pos, neighbour->pos, 0x3fffffff);
		}

		cGameRenderer::GetInstance()->DrawImmediate(gridPoint->pos, 0x3fffffff);
	}
#endif
}

std::vector<std::vector<int8_t>> cGameBoard::CreateGameBoard(int tileSize, int rows, int cols, int numJunctions, tVector2Df& entryPointF, tVector2Df& exitPointF)
{
	std::vector<std::vector<int8_t>> grid(rows, std::vector<int8_t>(cols));

	for (auto& gridLine : grid)
		memset(gridLine.data(), static_cast<int8_t>(eGridCellType::Empty), gridLine.size());

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

	grid[entryPoint.first][entryPoint.second]	= static_cast<int8_t>(eGridCellType::Walkable);
	grid[exitPoint.first][exitPoint.second]		= static_cast<int8_t>(eGridCellType::Walkable);

	auto linkJunctions = [&grid](const std::pair<int, int>& linkFrom, const std::pair<int, int>& linkTo)
		{
			const auto [linkToY, linkToX]		= linkTo;
			const auto [linkFromY, linkFromX]	= linkFrom;

			int yJunc = linkFromY;

			while (yJunc != linkToY)
			{
				grid[yJunc][linkFromX]	|= static_cast<int8_t>(eGridCellType::Walkable);
				yJunc += yJunc > linkToY ? -1 : 1;
			}

			int xJunc = linkFromX;

			while (xJunc != linkToX)
			{
				grid[linkToY][xJunc]	|= static_cast<int8_t>(eGridCellType::Walkable);
				xJunc += xJunc > linkToX ? -1 : 1;
			}
		};

	//generate a simple path through junctions connecting entry and exit points
	for (int i = 1; i < (int)junctions.size(); i++)
		linkJunctions(junctions[i], junctions[i - 1]);

	//generate link between junctions to create cycles
	for (int i = 1; i < (int)junctions.size() - 2; i++)
		linkJunctions(junctions[i], junctions[i + 1]);

	//generate buildable
	for (int y = 1; y < rows-1; y++)
	{
		for (int x = 1; x < cols-1; x++)
		{
			if (grid[y][x] == static_cast<int8_t>(eGridCellType::Empty)
				&& ( IsCellWalkable( grid[y - 1][x] )
					|| IsCellWalkable( grid[y + 1][x] )
					|| IsCellWalkable( grid[y][x - 1] )
					|| IsCellWalkable( grid[y][x + 1] )))
				grid[y][x] = static_cast<int8_t>(eGridCellType::Buildable);
		}
	}
	
	//debugging
	//for (int i = 1; i < (int)junctions.size(); i++)
	//{
	//	const auto& junction = junctions[i];

	//	grid[junction.first][junction.second] = 2;
	//}

	float smallError = 0.f;// tileSize * 0.2f;

	//yes inverted because I'm stupid
	entryPointF.y = static_cast<float>(entryPoint.first * tileSize + smallError );
	entryPointF.x = static_cast<float>(entryPoint.second * tileSize + smallError );

	exitPointF.y = static_cast<float>(exitPoint.first * tileSize + smallError );
	exitPointF.x = static_cast<float>(exitPoint.second * tileSize + smallError );

	return grid;
}

std::vector<std::vector<int8_t>> cGameBoard::CreateGameBoardWithDiagonalPathing(int tileSize, int rows, int cols, int numJunctions, tVector2Df& entryPointF, tVector2Df& exitPointF)
{
	std::vector<std::vector<int8_t>> grid(rows, std::vector<int8_t>(cols));

	for (auto& gridLine : grid)
		memset(gridLine.data(), static_cast<int8_t>(eGridCellType::Empty), gridLine.size());

	std::pair<int, int> entryPoint = { std::rand() % rows, 0 };

	std::vector<std::pair<int, int>> junctions;

	for (int i = 0; i < numJunctions; i++)
	{
		junctions.push_back({ (std::rand() % (rows - 2)) + 1, (std::rand() % (cols - 2)) + 1 });
	}

	std::sort(junctions.begin(), junctions.end(), [](const std::pair<int, int>& lh, const std::pair<int, int>& rh)
	{
		return lh.second < rh.second;
	});

	std::unordered_set<int> colUsed;

	for (int i = static_cast<int>(junctions.size()) - 1; i >= 0; i--)
	{
		if (colUsed.count(junctions[i].second) != 0)
		{
			junctions.erase( junctions.begin() + i );
			continue;
		}

		colUsed.insert(junctions[i].second);
	}

	junctions.insert(junctions.begin(), entryPoint);

	//exit point is last junction position projected on the right edge of the board
	std::pair<int, int> exitPoint = { std::rand() % rows, cols - 1 }; ;//junctions.back();
	//exitPoint.second = cols - 1;
	junctions.push_back(exitPoint);

	grid[entryPoint.first][entryPoint.second] = static_cast<int8_t>(eGridCellType::Walkable);
	grid[exitPoint.first][exitPoint.second] = static_cast<int8_t>(eGridCellType::Walkable);

	auto linkJunctionsManhatten = [&grid](const std::pair<int, int>& linkFrom, const std::pair<int, int>& linkTo)
	{
		const auto [linkToY, linkToX] = linkTo;
		const auto [linkFromY, linkFromX] = linkFrom;

		int yJunc = linkFromY;

		while (yJunc != linkToY)
		{
			grid[yJunc][linkFromX] |= static_cast<int8_t>(eGridCellType::Walkable);
			yJunc += yJunc > linkToY ? -1 : 1;
		}

		int xJunc = linkFromX;

		while (xJunc != linkToX)
		{
			grid[linkToY][xJunc] |= static_cast<int8_t>(eGridCellType::Walkable);
			xJunc += xJunc > linkToX ? -1 : 1;
		}
	};

	auto linkJunctions = [&grid, linkJunctionsManhatten](const std::pair<int, int>& linkFrom, const std::pair<int, int>& linkTo)
	{
		tVector2Df lastStart;
		tVector2Df start{ static_cast<float>(linkFrom.second), static_cast<float>(linkFrom.first) };
		tVector2Df end{ static_cast<float>(linkTo.second), static_cast<float>(linkTo.first) };
		tVector2Df dir{ directionNormalized( start, end ) };
		float step = 0.2f;
		const int numRows = static_cast<int>(grid.size());
		const int numCols = static_cast<int>(grid[0].size());

		while (distance(start, end) > 0.2f)
		{
			lastStart = start;
			start += dir * step;
			int lastGridX	= static_cast<int>(std::lround(lastStart.x));
			int lastGridY	= static_cast<int>(std::lround(lastStart.y));
			int gridX		= static_cast<int>(std::lround(start.x));
			int gridY		= static_cast<int>(std::lround(start.y));
			int gridAddY	= dir.x > 0 ? (gridY + 1 < numRows ? 1 : 0) : (gridY - 1 > 0 ? -1 : 0);
			int gridAddX	= dir.x < 0 ? (gridX + 1 < numCols ? 1 : 0) : (gridX - 1 > 0 ? -1 : 0);

			grid[gridY][gridX] |= static_cast<int8_t>(eGridCellType::Walkable);

			linkJunctionsManhatten( { lastGridY, lastGridX }, { gridY, gridX } );
		}
	};

	//generate a simple path through junctions connecting entry and exit points
	for (int i = 1; i < (int)junctions.size(); i++)
	{
		if (std::rand() % 2)
			linkJunctions(junctions[i-1], junctions[i]);
		else
			linkJunctionsManhatten(junctions[i - 1], junctions[i]);
	}

	//create cycles
	const int numCycles = std::rand() % ( ( numJunctions + 2 ) / 3 ); //entry & exit points are junctions

	for (int cycle = 0; cycle < numCycles; cycle++)
	{
		int startJunction	= 1 + std::rand() % (static_cast<int>(junctions.size()) - 1);
		int endJunction		= 1 + std::rand() % (static_cast<int>(junctions.size()) - 1);

		if( std::rand() % 2 )
			linkJunctions(junctions[startJunction], junctions[endJunction]);
		else
			linkJunctionsManhatten(junctions[startJunction], junctions[endJunction]);
	}
	//

	//generate buildable
	for (int y = 1; y < rows - 1; y++)
	{
		for (int x = 1; x < cols - 1; x++)
		{
			if (grid[y][x] == static_cast<int8_t>(eGridCellType::Empty)
				&& (   IsCellWalkable( grid[y - 1][x] )
					|| IsCellWalkable( grid[y + 1][x] )
					|| IsCellWalkable( grid[y][x - 1] )
					|| IsCellWalkable( grid[y][x + 1] )))
				grid[y][x] = static_cast<int8_t>(eGridCellType::Buildable);
		}
	}

	//debugging
	for (int i = 0; i < (int)junctions.size(); i++)
	{
		const auto& junction = junctions[i];

		grid[junction.first][junction.second] |= static_cast<int8_t>(eGridCellType::Junction);
	}

	float smallError = 0.f;// tileSize * 0.2f;

	//yes inverted because I'm stupid
	entryPointF.y = static_cast<float>(entryPoint.first * tileSize + smallError);
	entryPointF.x = static_cast<float>(entryPoint.second * tileSize + smallError);

	exitPointF.y = static_cast<float>(exitPoint.first * tileSize + smallError);
	exitPointF.x = static_cast<float>(exitPoint.second * tileSize + smallError);

	return grid;
}

std::vector<std::vector<int8_t>> cGameBoard::CreateGameBoardWFC(int tileSize, int rows, int cols, int junctions, tVector2Df& entryPoint, tVector2Df& exitPoint)
{
	struct wfcCell
	{
		int8_t		value		= 0;
		bool		collapsed	= false;
		int8_t		left		= -1;
		int8_t		top			= -1;
		int8_t		right		= -1;
		int8_t		bottom		= -1;
	};

	std::vector<std::vector<wfcCell>> wfcGrid(rows/3, std::vector<wfcCell>(cols/3));

	auto prioQueComp = [&wfcGrid](const std::pair<int, int>& a, const std::pair<int, int>& b)
	{
		const wfcCell& lh = wfcGrid[a.first][a.second];
		const wfcCell& rh = wfcGrid[b.first][b.second];

		int lPosibilities = ( lh.left < 0 ? 2 : 1 ) + (lh.top < 0 ? 2 : 1) + (lh.right < 0 ? 2 : 1) + (lh.bottom < 0 ? 2 : 1);
		int rPosibilities = ( rh.left < 0 ? 2 : 1 ) + (rh.top < 0 ? 2 : 1) + (rh.right < 0 ? 2 : 1) + (rh.bottom < 0 ? 2 : 1);

		return lPosibilities < rPosibilities;
	};

	std::priority_queue<std::pair<int, int>, std::vector<std::pair<int,int>>, decltype(prioQueComp)> openQueue( prioQueComp );

	openQueue.push( std::make_pair( rand()%(rows/3), rand()%(cols/3)));

	auto getRandomValueForConnection = [](int8_t left, int8_t top, int8_t right, int8_t bottom)
	{
		int8_t value = 0;

		value |= left < 0	? ((std::rand() % 2) << 3) : (left << 3);
		value |= top < 0	? ((std::rand() % 2) << 2) : (top << 2);
		value |= right < 0	? ((std::rand() % 2) << 1) : (right << 1);
		value |= bottom < 0	? ((std::rand() % 2) << 0) : (bottom << 0);

		return value;
	};


	while (!openQueue.empty())
	{
		auto [y, x] = openQueue.top(); openQueue.pop();
		wfcCell& cell = wfcGrid[y][x];

		cell.collapsed	= true;
		cell.value		= getRandomValueForConnection( cell.left, cell.top, cell.right, cell.bottom );

		if (x - 1 >= 0 && !wfcGrid[y][x - 1].collapsed)
		{
			wfcGrid[y][x-1].right	= (cell.value & (1 << 3)) > 0 ? 1 : 0;
			openQueue.push(std::make_pair(y, x-1));
		}

		if (x + 1 < cols/3 && !wfcGrid[y][x + 1].collapsed)
		{
			wfcGrid[y][x+1].left	= (cell.value & (1 << 1)) > 0 ? 1 : 0;
			openQueue.push(std::make_pair(y, x+1));
		}

		if (y - 1 >= 0 && !wfcGrid[y - 1][x].collapsed)
		{
			wfcGrid[y-1][x].bottom	= (cell.value & (1 << 2)) > 0 ? 1 : 0;
			openQueue.push(std::make_pair(y-1, x));
		}

		if (y + 1 < rows/3 && !wfcGrid[y + 1][x].collapsed)
		{
			wfcGrid[y+1][x].top		= (cell.value & (1 << 0)) > 0 ? 1 : 0;
			openQueue.push(std::make_pair(y+1, x));
		}
	}	
	
	std::vector<std::vector<int8_t>> grid(rows, std::vector<int8_t>(cols));

	for (auto& gridLine : grid)
		memset(gridLine.data(), static_cast<int8_t>(eGridCellType::Empty), gridLine.size());

	for (int y = 0; y < rows / 3; y++)
	{
		for (int x = 0; x < cols / 3; x++)
		{
			wfcCell& cell = wfcGrid[y][x];

			grid[y*3+1][x*3+1]	= cell.value != 0 ? static_cast<int8_t>(eGridCellType::Walkable) : static_cast<int8_t>(eGridCellType::Empty);
			grid[y*3+0][x*3+1]	= (cell.value & (1 << 2)) > 0 ? static_cast<int8_t>(eGridCellType::Walkable) : static_cast<int8_t>(eGridCellType::Empty);
			grid[y*3+2][x*3+1]	= (cell.value & (1 << 0)) > 0 ? static_cast<int8_t>(eGridCellType::Walkable) : static_cast<int8_t>(eGridCellType::Empty);
			grid[y*3+1][x*3+0]	= (cell.value & (1 << 3)) > 0 ? static_cast<int8_t>(eGridCellType::Walkable) : static_cast<int8_t>(eGridCellType::Empty);
			grid[y*3+1][x*3+2]	= (cell.value & (1 << 1)) > 0 ? static_cast<int8_t>(eGridCellType::Walkable) : static_cast<int8_t>(eGridCellType::Empty);
		}
	}

	auto getFurthestEndCell = []( const std::vector<std::vector<int8_t>>& grid, const std::pair<int, int>& startCell )
	{
		const int									rows		= static_cast<int>( grid.size() );
		const int									cols		= static_cast<int>( grid[0].size() );
		int											maxDist		= -1;
		std::pair<int,int>							farthest	= startCell;
		std::queue<std::pair<int,int>>				frontier;
		std::set<std::pair<int, int>>				visitMap;
		const std::array<std::pair<int,int>, 4>	neighbourOffsets = 
		{
			std::make_pair(-1, 0),
			{ 1, 0 },
			{ 0, -1 },
			{ 0, 1 }
		};

		frontier.push(startCell);
		visitMap.insert( startCell );

		while (!frontier.empty())
		{
			auto [x, y] = frontier.front();
			frontier.pop();

			for (auto [xOffset, yOffset] : neighbourOffsets)
			{
				if (x + xOffset < 0 || x + xOffset >= cols
					|| y + yOffset < 0 || y + yOffset >= rows)
					continue;

				if (auto neighbour = std::make_pair(x + xOffset, y + yOffset); visitMap.count(neighbour) == 0u && IsCellWalkable(grid[y + yOffset][x + xOffset]) )
				{
					frontier.push(neighbour);
					visitMap.insert(neighbour );

					if (int dist = distance(tVector2D<int>(startCell), tVector2D<int>(neighbour)); dist > maxDist)
					{
						maxDist		= dist;
						farthest	= neighbour;
					}
				}
			}
		}

		return farthest;
	};

	for (int y = 0; y < rows; y++)
	{
		//for (int x = 0; x < cols; x++)
		{
			if (IsCellWalkable( grid[y][0] ))
			{
				entryPoint.y = static_cast<float>(y * tileSize);
				entryPoint.x = static_cast<float>(0 * tileSize);

				auto [farX, farY] = getFurthestEndCell( grid, {0, y} );

				exitPoint.y = static_cast<float>(farY * tileSize);
				exitPoint.x = static_cast<float>(farX * tileSize);

				break;
			}
		}
	}

	//generate buildable
	for (int y = 1; y < rows - 1; y++)
	{
		for (int x = 1; x < cols - 1; x++)
		{
			if (grid[y][x] == static_cast<int8_t>(eGridCellType::Empty)
				&& (IsCellWalkable(grid[y - 1][x])
					|| IsCellWalkable(grid[y + 1][x])
					|| IsCellWalkable(grid[y][x - 1])
					|| IsCellWalkable(grid[y][x + 1])))
				grid[y][x] = static_cast<int8_t>(eGridCellType::Buildable);
		}
	}

	return grid;
}

bool cGameBoard::IsCellWalkable(int8_t cellValue)
{
	return ((cellValue & static_cast<int8_t>(eGridCellType::Walkable)) != 0);
}

bool cGameBoard::IsBuildable(int8_t cellValue)
{
	return ((cellValue & static_cast<int8_t>(eGridCellType::Buildable)) != 0);
}
