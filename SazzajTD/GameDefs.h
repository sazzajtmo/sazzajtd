#ifndef GAME_DEFS_H
#define GAME_DEFS_H

enum class eGameObjectTypes
{
	None = 0,
	Board,
	Player,
	Enemy,
	Building,
	GunBuilding
};

namespace GameConfig
{
	static struct
	{
		float		enemy_hp				= 100.f;
		float		enemy_speed				= 100.f;
		float		enemy_spawn_rate		= 5.f;	//enemies per second

		float		building_slow_radius	= 60.f;
		float		building_slow_factor	= 0.2f;

		float		building_gun_radius		= 60.f;
		float		building_gun_damage		= 10.f;
		float		building_gun_firerate	= 5.f;	//shot per second

	} values;
}


#endif