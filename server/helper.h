#pragma once
#include <iostream>
#pragma warning(push)
#pragma warning(disable: 4005)
#pragma warning(disable: 4251)
#pragma warning(disable: 4996)
#include "../protobuf_mud_lib/mud_lib.pb.h"
#pragma warning(pop)

inline std::string getDirection(mud::direction direction)
{
	switch (direction)
	{
	case mud::direction::EAST:
		return "EAST";
	case mud::direction::WEST:
		return "WEST";
	case mud::direction::NORTH:
		return "NORTH";
	case mud::direction::SOUTH:
		return "SOUTH";
	}
}

//Player
inline std::ostream& operator<<  (
	std::ostream& os,
	const mud::player& player)
{
	os << "name: " << player.name() << std::endl;
	os << "password: " << player.passwordhash() << std::endl;
	os << "id: " << player.id() << std::endl;
	for (const auto& id : player.idcharacters())
	{
		os << "\t" << id << std::endl;
	}
	return os;
}

//Tile
inline std::ostream& operator<<  (
	std::ostream& os,
	const mud::tile& tile)
{
	os << "id: " << tile.id() << std::endl;
	os << "type: " << tile.type() << std::endl;
	os << "occupant type: " << tile.occupant_type() << std::endl;
	os << "occupant id: " << tile.occupant_id() << std::endl;
	os << "neighbours: " << std::endl;
	for (const auto& t : tile.neighbours())
	{
		os << "\tDirection: " << getDirection(t.neighbour_direction()) << std::endl;
		os << "\tTile: " << t.neighbour_tile_id() << std::endl;
	}
	return os;
}

//Character
inline std::ostream& operator<<  (
	std::ostream& os,
	const mud::character& character)
{
	os << "id: " << character.id() << std::endl;
	os << "name: " << character.name() << std::endl;
	os << "tile: " << character.tile_id() << std::endl;
	os << "facing: " << getDirection(character.facing()) << std::endl;
	os << "state: " << character.state() << std::endl;
	return os;
}

//Enemy
inline std::ostream& operator<<  (
	std::ostream& os,
	const mud::enemy& enemy)
{
	os << "id: " << enemy.id() << std::endl;
	os << "name: " << enemy.name() << std::endl;
	os << "tile: " << enemy.tile_id() << std::endl;
	os << "facing: " << getDirection(enemy.facing()) << std::endl;
	os << "state: " << enemy.state() << std::endl;
	return os;
}

