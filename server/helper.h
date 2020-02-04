#pragma once
#include <iostream>
#pragma warning(push)
#pragma warning(disable: 4005)
#pragma warning(disable: 4251)
#pragma warning(disable: 4996)
#include "../protobuf_mud_lib/mud_lib.pb.h"
#pragma warning(pop)
#include <google/protobuf/util/json_util.h>

static mud::direction dirNorth = []()
{
	mud::direction dir{};
	dir.set_value(mud::direction_direction_enum_NORTH);
	return dir;
}();

static mud::direction dirSouth = []()
{
	mud::direction dir{};
	dir.set_value(mud::direction_direction_enum_SOUTH);
	return dir;
}();

static mud::direction dirWest = []()
{
	mud::direction dir{};
	dir.set_value(mud::direction_direction_enum_WEST);
	return dir;
}();

static mud::direction dirEast = []()
{
	mud::direction dir{};
	dir.set_value(mud::direction_direction_enum_EAST);
	return dir;
}();

inline std::string getDirection(mud::direction direction)
{
	switch (direction.value())
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

inline mud::direction_direction_enum turnLeft(mud::direction_direction_enum direction)
{
	switch (direction)
	{
	case mud::direction_direction_enum_EAST:
		return mud::direction_direction_enum_NORTH;
	case mud::direction::NORTH:
		return mud::direction_direction_enum_WEST;
	case mud::direction::WEST:
		return mud::direction_direction_enum_SOUTH;
	case mud::direction::SOUTH:
		return mud::direction_direction_enum_EAST;
	}
}

inline mud::direction_direction_enum turnRight(mud::direction_direction_enum direction)
{
	switch (direction)
	{
	case mud::direction_direction_enum_EAST:
		return mud::direction_direction_enum_SOUTH;
	case mud::direction::NORTH:
		return mud::direction_direction_enum_EAST;
	case mud::direction::WEST:
		return mud::direction_direction_enum_NORTH;
	case mud::direction::SOUTH:
		return mud::direction_direction_enum_WEST;
	}
}

template<typename Entity>
mud::attribute& getAttribute(Entity& entity, mud::attribute::attribute_name attributeName)
{
	for (mud::attribute& a : *entity.mutable_attributes())
	{
		if (a.name() == attributeName) 
		{
			return a;
		}
	}
}

template<typename Book>
std::string BookToJson(const Book& book)
{
	std::string out;
	google::protobuf::util::JsonOptions options{};
	options.add_whitespace = true;
	options.always_print_primitive_fields = true;
	auto status = 
		google::protobuf::util::MessageToJsonString(
			book,
			&out,
			options);

	if (!status.ok())
	{
		std::cout << status << std::endl;
		return "";
	}

	return out;
}

template<typename Book>
inline bool JsonToBook(std::string contents, Book& book)
{
	google::protobuf::util::JsonParseOptions options{};
	options.ignore_unknown_fields = true;
	auto status = google::protobuf::util::JsonStringToMessage(
		contents,
		&book,
		options);

	if (!status.ok())
	{
		std::cout << status << std::endl;
		return false;
	}

	return true;
}


template<typename Book>
bool saveBook(std::string path, const Book& book)
{
	std::string contents = BookToJson(book);

	std::ofstream ofs(
		path,
		std::ios::out | std::ios::binary);

	ofs << contents;
	ofs.close();

	return true;
}

template<typename Book>
bool loadBook(std::string path, Book& book)
{
	std::ifstream ifs(
		path,
		std::ios::in | std::ios::binary);
	if (!ifs) {
		std::cerr << "error reading " << path << std::endl;
		return false;
	}
	else {
		std::string contents(std::istreambuf_iterator<char>(ifs), {});
		return JsonToBook(contents, book);
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
	os << "state: " << character.state().value() << std::endl;
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
	os << "state: " << enemy.state().value() << std::endl;
	return os;
}

