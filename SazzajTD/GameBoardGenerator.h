#ifndef GAME_BOARD_GENERATOR_H
#define GAME_BOARD_GENERATOR_H

#include "GameObject.h"
#include "GameDefs.h"
#include <vector>

namespace GameBoardGenerator
{
	std::vector<std::vector<int8_t>> CreateGameBoard( int tileSize, int rows, int cols, int junctions, tVector2Df& entryPoint, tVector2Df& exitPoint );
};

#endif