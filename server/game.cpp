#include "game.h"

void Game::run()
{
	loadData();

	if (tiles.size() == 0)
	{
		initMap();
		saveTiles();
	}

	for (auto& field : tiles)
	{
		std::cout << field.second << std::endl;
	}

	showCharacters();
	std::cout << "Select your character or type \"new\":" << std::endl;

	characterId = -1;
	std::string characterName;
	while (characterId == -1)
	{
		std::cin >> characterName;
		if (characterName == "new")
		{
			createCharacter();
		}
		else 
		{
			for (auto& field : characters)
			{
				if (field.second.name() == characterName)
				{
					characterId = field.first;
					break;
				}
			}
		}
	}
	bool loop = true;
	while(loop)
	{
		Keyboard keyboard;

		Keyboard::Input input = keyboard.GetInput();
		
		processCharacter(input);

		Sleep(1000);
	}
}

void Game::initMap()
{
	// 1 2
	// 3
	mud::tile tile1 = createTile(mud::tile_type::EMPTY, "tile 1");
	mud::tile tile2 = createTile(mud::tile_type::EMPTY, "tile 2");
	mud::tile tile3 = createTile(mud::tile_type::EMPTY, "tile 3");

	addNeighbour(tiles[tile1.id()], tiles[tile2.id()], mud::direction::EAST);
	addNeighbour(tiles[tile1.id()], tiles[tile3.id()], mud::direction::SOUTH);

}

static int idNextTile = 0;
mud::tile Game::createTile(mud::tile_type type, std::string mood)
{
	mud::tile tile{};
	tile.set_id(++idNextTile);
	tile.set_type(type);
	tile.set_occupant_type(mud::resident_type::NOBODY);
	tile.set_occupant_id(0);
	tile.set_mood(mood);

	tiles.insert({ tile.id(), tile });

	return tile;
}

void Game::addNeighbour(mud::tile& tile1, mud::tile& tile2, mud::direction direction)
{
	mud::neighbour neighbour{};
	neighbour.set_neighbour_direction(direction);
	neighbour.set_neighbour_tile_id(tile2.id());

	mud::neighbour neighbour2{};
	neighbour2.set_neighbour_direction(getOppositeDirection(direction));
	neighbour2.set_neighbour_tile_id(tile1.id());

	*tile1.add_neighbours() = neighbour;
	*tile2.add_neighbours() = neighbour2;
}


void Game::loadData()
{
	mud::tile_book tilesBook{};
	mud::enemy_book enemyBook{};
	mud::character_book characterBook{};
	std::ifstream ifs(
		"tiles.data",
		std::ios::in | std::ios::binary);
	if (!ifs) {
		std::cerr << "error reading tiles" << std::endl;
	}
	else {
		tilesBook.ParseFromIstream(&ifs);
	}

	std::ifstream ifs2(
		"enemies.data",
		std::ios::in | std::ios::binary);
	if (!ifs2) {
		std::cerr << "error reading enemies" << std::endl;
	}
	else {
		enemyBook.ParseFromIstream(&ifs2);
	}

	std::ifstream ifs3(
		"characters.data",
		std::ios::in | std::ios::binary);
	if (!ifs3) {
		std::cerr << "error reading characters" << std::endl;
	}
	else {
		characterBook.ParseFromIstream(&ifs3);
	}

	for (const auto& t : tilesBook.tiles())
	{
		tiles.insert({ t.id(), t });
	}
	for (const auto& e : enemyBook.enemies())
	{
		enemies.insert({ e.id(), e });
	}
	for (const auto& c : characterBook.characters())
	{
		characters.insert({ c.id(), c });
	}
}

bool Game::saveData()
{
	bool flag = true;
	flag = saveTiles() && flag;
	flag = saveEnemies() && flag;
	flag = saveCharacters() && flag;

	return flag;
}

bool Game::saveTiles()
{
	mud::tile_book tilesBook{};
	for (auto& field : tiles)
	{
		*tilesBook.add_tiles() = field.second;
	}
	std::ofstream ofs(
		"tiles.data",
		std::ios::out | std::ios::binary);

	if (!tilesBook.SerializeToOstream(&ofs)) {
		std::cerr << "error writing tiles" << std::endl;
		return false;
	}

	return true;
}


bool Game::saveCharacters()
{
	mud::character_book characterBook{};

	for (auto& field : characters)
	{
		*characterBook.add_characters() = field.second;
	}

	std::ofstream ofs(
		"characters.data",
		std::ios::out | std::ios::binary);
	if (!characterBook.SerializeToOstream(&ofs)) {
		std::cerr << "error writing characters" << std::endl;
		return false;
	}

	return true;
}


bool Game::saveEnemies()
{
	mud::enemy_book enemyBook{};

	for (auto& field : enemies)
	{
		*enemyBook.add_enemies() = field.second;
	}

	std::ofstream ofs(
		"enemies.data",
		std::ios::out | std::ios::binary);

	if (!enemyBook.SerializeToOstream(&ofs)) {
		std::cerr << "error writing enemies" << std::endl;
		return false;
	}

	return true;
}

mud::direction Game::getOppositeDirection(mud::direction dir)
{
	switch (dir) 
	{
	case mud::direction::EAST:
		return mud::direction::WEST;
	case mud::direction::WEST:
		return mud::direction::EAST;
	case mud::direction::NORTH:
		return mud::direction::SOUTH;
	case mud::direction::SOUTH:
		return mud::direction::NORTH;
	}

	std::cerr << "Error: unknown direction" << std::endl;
	return mud::direction();
}

void Game::showCharacters()
{
	std::cout << "Characters: " << std::endl;
	for (auto& field : characters)
	{
		std::cout << field.second << std::endl;
	}
}

bool Game::createCharacter()
{
	int maxId = 0;
	for (const auto& field : characters)
	{
		if (field.first > maxId) {
			maxId = field.first;
		}
	}

	mud::character character{};
	std::string inputName;
	std::cout << "Enter your character's name:" << std::endl;
	bool correctName = false;
	while (!correctName)
	{
		std::cin >> inputName;
		character.set_name(inputName);
		correctName = true;
		for (const auto& field : characters)
		{
			if (field.second.name() == character.name())
			{
				std::cout << "This character already exist!" << std::endl;
				correctName = false;
				break;
			}
		}
	}
	character.set_id(maxId + 1);
	character.set_facing(mud::direction::SOUTH);
	character.set_state(mud::character_state::NONE);
	for (auto& field : tiles)
	{
		if (field.second.occupant_type() == mud::resident_type::NOBODY)
		{
			character.set_tile_id(field.first);
			field.second.set_occupant_type(mud::resident_type::CHARACTER);
			field.second.set_occupant_id(character.id());
			break;
		}
	}

	characterId = character.id();
	characters.insert({ characterId, character });

	std::cout << "Adding character:" << std::endl;
	std::cout << character << std::endl;

	return saveCharacters();
}

void Game::processCharacter(Keyboard::Input input)
{
	mud::character character = characters[characterId];
	mud::tile tile = tiles[character.tile_id()];
	mud::tile destination;
	bool foundDestination = false;
	switch (input) 
	{
	case Keyboard::Forward:

		for (const auto& neighbour : tile.neighbours()) {
			if (neighbour.neighbour_direction() == characters[characterId].facing())
			{
				destination = tiles[neighbour.neighbour_tile_id()];
				foundDestination = true;
				break;
			}
		}

		if (foundDestination) 
		{
			if (movingCharacter(characters[characterId], destination)) 
			{
				std::cout << "Moving to " << destination.id() << std::endl;
			}
			else
			{
				std::cout << "You can't move to " << destination.id() << std::endl;
			}
		}
		else
		{
			std::cout << "No tile forward you " << std::endl;
		}
		break;

	case Keyboard::TurnLeft:
		switch (characters[characterId].facing())
		{
		case mud::direction::EAST:
			characters[characterId].set_facing(mud::direction::NORTH);
			std::cout << "You face north" << std::endl;
			break;
		case mud::direction::NORTH:
			characters[characterId].set_facing(mud::direction::WEST);
			std::cout << "You face west" << std::endl;
			break;
		case mud::direction::WEST:
			characters[characterId].set_facing(mud::direction::SOUTH);
			std::cout << "You face south" << std::endl;
			break;
		case mud::direction::SOUTH:
			characters[characterId].set_facing(mud::direction::EAST);
			std::cout << "You face east" << std::endl;
			break;
		}
		break;

	case Keyboard::TurnRight:
		switch (characters[characterId].facing())
		{
		case mud::direction::EAST:
			characters[characterId].set_facing(mud::direction::SOUTH);
			std::cout << "You face south" << std::endl;
			break;
		case mud::direction::NORTH:
			characters[characterId].set_facing(mud::direction::EAST);
			std::cout << "You face east" << std::endl;
			break;
		case mud::direction::WEST:
			characters[characterId].set_facing(mud::direction::NORTH);
			std::cout << "You face north" << std::endl;
			break;
		case mud::direction::SOUTH:
			characters[characterId].set_facing(mud::direction::WEST);
			std::cout << "You face west" << std::endl;
			break;
		}
		break;

	case Keyboard::Info:
		std::cout << std::endl << "Character:" << std::endl;
		std::cout << characters[characterId] << std::endl;
		std::cout << std::endl << "Tile:" << std::endl;
		std::cout << tiles[characters[characterId].tile_id()] << std::endl;
		break;
	}
}

bool Game::movingCharacter(mud::character character, mud::tile tile)
{
	if (tile.occupant_type() == mud::resident_type::NOBODY)
	{
		mud::tile currentTile = tiles[characters[characterId].tile_id()];
		currentTile.set_occupant_id(0);
		currentTile.set_occupant_type(mud::resident_type::NOBODY);
		tile.set_occupant_id(character.id());
		tile.set_occupant_id(mud::resident_type::CHARACTER);
		characters[characterId].set_tile_id(tile.id());
		return true;
	}

	return false;
}
