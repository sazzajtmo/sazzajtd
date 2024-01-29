#ifndef GAME_DEFS_H
#define GAME_DEFS_H

#include <array>
#include <string>

enum class eGameState : int
{
	StartUp = 0,
	Playing,
	Paused
};

enum class eGameObjectTypes
{
	None = 0,
	Board,
	Static,	//rendered texture, nothing more

	Buildings_Start,
	Building = Buildings_Start,
	GunBuilding,
	Buildings_End = GunBuilding,

	Player,
	Enemy
};

enum class eGameScoreSource : int
{
	EnemyDeath = 0,
	AllyDeath,
	Time,
	Count
};

enum class eGameBoardType : int
{
	SimpleLoops,
	HybridDiagonals,
	WaveFunctionCollapse
};

namespace GameConfig
{
	static struct
	{
		int					buildings				= 20;

		float				enemy_hp				= 100.f;
		float				enemy_speed				= 80.f;
		float				enemy_spawn_rate		= 2.f;	//enemies per second

		float				building_slow_radius	= 60.f;
		float				building_slow_factor	= 0.2f;

		float				building_gun_radius		= 60.f;
		float				building_gun_damage		= 10.f;
		float				building_gun_firerate	= 5.f;	//shot per second

	} values;

	//less safe than a map
	static std::array<float, static_cast<int>(eGameScoreSource::Count)> scoring =
	{
		50.f, //EnemyDeath = 0,
		-10.f, //AllyDeath,
		10.f //Time - per second
	};

	//less safe than a map
	static std::array<std::string, static_cast<int>(eGameScoreSource::Count)> scoreSourceNames =
	{
		"EnemyDeath", //EnemyDeath = 0,
		"AllyDeath", //AllyDeath,
		"Lifetime score per second" //Time - per second
	};
}

#endif