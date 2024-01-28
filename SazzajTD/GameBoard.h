#ifndef GAME_BOARD_H
#define GAME_BOARD_H

#include "GameObject.h"
#include "Utilities.h"
#include <vector>
#include <string>

class cAnimatedTexture;

class cGameBoard : public cGameObject
{
public:
	enum class eGridCellType : int8_t
	{
		Empty		= 0,
		Walkable	= ( 1 << 0 ),
		Buildable	= ( 1 << 1 ),
		Junction	= ( 1 << 2 )
	};

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
			
			void								Cleanup() override;

			void								Init() override;
			void								InitPathfinding( const std::string& walkableMapTextureFilePath );
			void								InitPathfinding( const std::vector<std::vector<int8_t>>& grid, int tileSize );
			tPoint*								FindGridPoint( float x, float y, float tolerance = 10.f ) const;

			std::vector<tVector2Df>				FindPathBFS( const tVector2Df& start, const tVector2Df& end ) const;
			std::vector<tVector2Df>				FindPathAstar( const tVector2Df& start, const tVector2Df& end ) const;

			void								Update( float deltaTime ) override;
			void								Draw() override;
			void								DrawDebug() override;

	inline	const tVector2Df&					GetEntryPoint() const	{ return m_entryPoint; }
	inline	const tVector2Df&					GetExitPoint() const	{ return m_exitPoint; }

	static	std::vector<std::vector<int8_t>>	CreateGameBoard(int tileSize, int rows, int cols, int junctions, tVector2Df& entryPoint, tVector2Df& exitPoint);
	static	std::vector<std::vector<int8_t>>	CreateGameBoardWithDiagonalPathing(int tileSize, int rows, int cols, int junctions, tVector2Df& entryPoint, tVector2Df& exitPoint);
	static	std::vector<std::vector<int8_t>>	CreateGameBoardWFC(int tileSize, int rows, int cols, int junctions, tVector2Df& entryPoint, tVector2Df& exitPoint);

	inline	std::string							GetBoardName() const	{ return m_boardName;  }

private:
	static	bool								IsCellWalkable( int8_t cellValue );
	static	bool								IsBuildable( int8_t cellValue );

private:
			std::vector<std::vector<int8_t>>	m_grid;
			std::vector<tPoint*>				m_walkPoints;
			tPoint								m_nullPoint;

			tVector2Df							m_entryPoint;
			tVector2Df							m_exitPoint;

			std::unique_ptr<cAnimatedTexture>	m_startPointModel;
			std::unique_ptr<cAnimatedTexture>	m_endPointModel;

			std::string							m_boardName	= "";

			int									m_mouseCbId = -1;
			tVector2D<int>						m_selectedCell{ -1, -1 };
			bool								m_selectedCellBuildable{ false };
		
};

#endif