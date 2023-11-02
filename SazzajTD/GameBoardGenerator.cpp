#include "GameBoardGenerator.h"
#include <cmath>
#include <algorithm>
#include <queue>
#include "GameRenderer.h"
#include "MemHelper.h"

std::vector<std::vector<int8_t>> GameBoardGenerator::CreateGameBoard(int tileSize, int rows, int cols, int numJunctions, tVector2Df& entryPointF, tVector2Df& exitPointF)
{
	std::vector<std::vector<int8_t>> grid( rows, std::vector<int8_t>(cols) );

	for( auto& gridLine : grid )
		memset( gridLine.data(), 0, gridLine.size() );

	std::pair<int,int> entryPoint	= { std::rand() % rows, 0 };

	std::vector<std::pair<int,int>> junctions;

	for (int i = 0; i < numJunctions; i++)
	{
		junctions.push_back( { ( std::rand() % ( rows - 2 ) ) + 1, ( std::rand() % ( cols - 2 ) ) + 1 } );
	}

	std::sort( junctions.begin(), junctions.end(), [entryPoint]( const std::pair<int, int>& lh, const std::pair<int, int>& rh )
	{
		return (abs(entryPoint.first - lh.first) + abs(entryPoint.second - lh.second)) < (abs(entryPoint.first - rh.first) + abs(entryPoint.second - rh.second));
	});

	junctions.insert( junctions.begin(), entryPoint);

	//exit point is last junction position projected on the right edge of the board
	std::pair<int,int> exitPoint	= junctions.back();
	exitPoint.second = cols - 1;
	junctions.push_back( exitPoint );

	grid[entryPoint.first][entryPoint.second]	= 1;
	grid[exitPoint.first][exitPoint.second]		= 1;

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
	for (int i = 1; i < (int) junctions.size(); i++ )
		linkJunctions( junctions[i], junctions[i - 1]);

	//generate link between junctions to create cycles
	for (int i = 1; i < (int) junctions.size() - 2; i++ )
		linkJunctions( junctions[i], junctions[i+1]);

	//debugging
	for (int i = 1; i < (int)junctions.size(); i++)
	{
		const auto& junction = junctions[i];

		grid[junction.first][junction.second] = 2;
	}

	cGameRenderer::GetInstance()->ExportGridToFile( grid, tileSize );

	//yes inverted because I'm stupid
	entryPointF.y = static_cast<float>( entryPoint.first * tileSize + tileSize / 2 );
	entryPointF.x = static_cast<float>( entryPoint.second * tileSize + tileSize / 2 );

	exitPointF.y = static_cast<float>( exitPoint.first * tileSize + tileSize / 2 );
	exitPointF.x = static_cast<float>( exitPoint.second * tileSize + tileSize / 2 );

	return grid;
}
