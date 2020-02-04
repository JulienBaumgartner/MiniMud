#include "game.h"

void Game::run()
{
	loadData();

	if (tiles.size() == 0)
	{
		initMap();
		saveBook("tiles.json", getTileBook());
	}

	/*if (enemies.size() == 0)
	{
		mud::enemy enemy;
		enemy.set_name("slime A");
		enemy.set_id(1);
		enemy.set_tile_id(8);
		*enemy.mutable_facing() = dirSouth;

		mud::attribute life;
		life.set_name(mud::attribute_attribute_name_LIFE);
		life.set_value(20);
		life.set_regen(2);
		*enemy.add_attributes() = life;

		mud::attribute strenght;
		strenght.set_name(mud::attribute_attribute_name_STRENGTH);
		strenght.set_value(10);
		*enemy.add_attributes() = strenght;

		enemies.insert({ enemy.id(), enemy });

		mud::enemy enemy2;
		enemy2.set_name("slime B");
		enemy2.set_id(2);
		enemy2.set_tile_id(9);
		*enemy2.mutable_facing() = dirSouth;
		*enemy2.add_attributes() = life;
		*enemy2.add_attributes() = strenght;

		enemies.insert({ enemy2.id(), enemy2 });

		saveBook("enemies.json", getEnemyBook());
	}*/

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
	keyboard.run();
	bool loop = true;
	while(loop)
	{

		input_t input = execute_keyboard();
		
		processCharacter(input);

		for (auto& enemy : enemies)
		{
			processEnemy(enemy.second);
		}

		for (auto& character : characters)
		{
			auto& characterLife = getAttribute(character.second, mud::attribute::LIFE);
			if (characterLife.value() == 0)
			{
				std::cout << "You are dead" << std::endl;
				loop = false;
			}
		}

		Sleep(1000);
	}
	keyboard.stop();
}

input_t Game::execute_keyboard()
{
	input_t entry = input_t::NONE;
	// Get keyboard entries.
	for (const auto& field : keyboard.input_key)
	{
		if (keyboard.check_released_input(field.first))
		{
			return field.first;
		}
	}
	return entry;
}

void Game::initMap()
{
	// 5 6   8
	// 4 1 2 7
	//   3
	//   9
	mud::tile tile1 = createTile(mud::tile::EMPTY, "tile 1");
	mud::tile tile2 = createTile(mud::tile::EMPTY, "tile 2");
	mud::tile tile3 = createTile(mud::tile::EMPTY, "tile 3");
	mud::tile tile4 = createTile(mud::tile::EMPTY, "tile 4");
	mud::tile tile5 = createTile(mud::tile::EMPTY, "tile 5");
	mud::tile tile6 = createTile(mud::tile::EMPTY, "tile 6");
	mud::tile tile7 = createTile(mud::tile::EMPTY, "tile 7");
	mud::tile tile8 = createTile(mud::tile::EMPTY, "tile 8");
	mud::tile tile9 = createTile(mud::tile::EMPTY, "tile 9");

	addNeighbour(tiles[tile1.id()], tiles[tile2.id()], mud::direction::EAST);
	addNeighbour(tiles[tile1.id()], tiles[tile3.id()], mud::direction::SOUTH);
	addNeighbour(tiles[tile1.id()], tiles[tile4.id()], mud::direction::WEST);
	addNeighbour(tiles[tile1.id()], tiles[tile6.id()], mud::direction::NORTH);
	addNeighbour(tiles[tile5.id()], tiles[tile6.id()], mud::direction::EAST);
	addNeighbour(tiles[tile5.id()], tiles[tile4.id()], mud::direction::SOUTH);
	addNeighbour(tiles[tile2.id()], tiles[tile7.id()], mud::direction::EAST);
	addNeighbour(tiles[tile7.id()], tiles[tile8.id()], mud::direction::NORTH);
	addNeighbour(tiles[tile3.id()], tiles[tile9.id()], mud::direction::SOUTH);

}

static int idNextTile = 0;
mud::tile Game::createTile(mud::tile_tile_type type, std::string mood)
{
	mud::tile tile{};
	tile.set_id(++idNextTile);
	tile.set_type(type);
	tile.set_occupant_type(mud::tile::NOBODY);
	tile.set_occupant_id(0);
	tile.set_mood(mood);

	tiles.insert({ tile.id(), tile });

	return tile;
}

void Game::addNeighbour(mud::tile& tile1, mud::tile& tile2, mud::direction_direction_enum dir)
{
	mud::neighbour neighbour{};
	mud::direction direction{};
	direction.set_value(dir);
	*neighbour.mutable_neighbour_direction() = direction;
	neighbour.set_neighbour_tile_id(tile2.id());

	mud::neighbour neighbour2{};
	mud::direction direction2{};
	direction2.set_value(getOppositeDirection(dir));
	*neighbour2.mutable_neighbour_direction() = direction2;
	neighbour2.set_neighbour_tile_id(tile1.id());

	*tile1.add_neighbours() = neighbour;
	*tile2.add_neighbours() = neighbour2;
}


void Game::loadData()
{
	mud::tile_book tilesBook{};
	mud::enemy_book enemyBook{};
	mud::character_book characterBook{};

	loadBook("tiles.json", tilesBook);
	loadBook("enemies.json", enemyBook);
	loadBook("characters.json", characterBook);

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
	flag = saveBook("tiles.json", getTileBook()) && flag;
	flag = saveBook("enemies.json", getEnemyBook()) && flag;
	flag = saveBook("characters.json", getCharacterBook()) && flag;

	return flag;
}

mud::direction_direction_enum Game::getOppositeDirection(mud::direction_direction_enum dir)
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
	return mud::direction_direction_enum();
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
	mud::direction direction{};
	direction.set_value(mud::direction::SOUTH);
	*character.mutable_facing() = direction;
	mud::character_state state{};
	state.set_value(mud::character_state::NONE);
	*character.mutable_state() = state;
	for (auto& field : tiles)
	{
		if (field.second.occupant_type() == mud::tile::NOBODY)
		{
			character.set_tile_id(field.first);
			field.second.set_occupant_type(mud::tile::CHARACTER);
			field.second.set_occupant_id(character.id());
			break;
		}
	}

	mud::attribute life;
	life.set_name(mud::attribute::LIFE);
	life.set_value(100);
	life.set_regen(1);
	*character.add_attributes() = life;

	mud::attribute strenght;
	strenght.set_name(mud::attribute::STRENGTH);
	strenght.set_value(10);
	*character.add_attributes() = strenght;


	characterId = character.id();
	characters.insert({ characterId, character });

	std::cout << "Adding character:" << std::endl;
	std::cout << character << std::endl;

	return saveBook("characters.json", getCharacterBook());
}

void Game::processCharacter(input_t input)
{
	mud::character& character = characters[characterId];
	mud::tile& tile = tiles[character.tile_id()];
	mud::tile destination;
	mud::direction direction{};
	std::int64_t facingTileId = 0;
	bool foundDestination = false;
	switch (input) 
	{
	case input_t::ATTACK:

		for (auto& neighbour : tile.neighbours())
		{
			if (neighbour.neighbour_direction().value() == character.facing().value())
			{
				facingTileId = neighbour.neighbour_tile_id();
				break;
			}
		}

		if (facingTileId > 0)
		{
			mud::tile& facingTile = tiles[facingTileId];
			if (facingTile.occupant_type() == mud::tile::NOBODY)
			{
				std::cout << "You hit nothing" << std::endl;
			}
			if (facingTile.occupant_type() == mud::tile::ENEMY)
			{
				mud::enemy& enemy = enemies[facingTile.occupant_id()];
				std::cout << "\a" << std::flush;
				std::cout << character.name() << " attack " << enemy.name() << std::endl;

				auto& enemyLife = getAttribute(enemy, mud::attribute::LIFE);
				auto& characterStrength = getAttribute(character, mud::attribute::STRENGTH);
				hit(enemyLife, characterStrength.value());
				if (enemyLife.value() == 0)
				{
					std::cout << enemy.name() << " is dead!" << std::endl;
					deleteEnemy(enemy.id());
				}
			}
		}
		else
		{
			std::cout << "You hit nothing" << std::endl;
		}
		break;
	case input_t::FORWARD:

		for (const auto& neighbour : tile.neighbours()) {
			if (neighbour.neighbour_direction().value() == characters[characterId].facing().value())
			{
				destination = tiles[neighbour.neighbour_tile_id()];
				foundDestination = true;
				break;
			}
		}

		if (foundDestination) 
		{
			if (movingCharacter(characterId, destination.id())) 
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

	case input_t::LEFT:
		direction.set_value(turnLeft(character.facing().value()));
		*character.mutable_facing() = direction;
		std::cout << "You face " << getDirection(direction) << std::endl;
		break;

	case input_t::RIGHT:
		direction.set_value(turnRight(character.facing().value()));
		*character.mutable_facing() = direction;
		std::cout << "You face " << getDirection(direction) << std::endl;
		break;

	case input_t::INFO:
		std::cout << std::endl << "Character:" << std::endl;
		std::cout << characters[characterId] << std::endl;
		std::cout << std::endl << "Tile:" << std::endl;
		std::cout << tiles[characters[characterId].tile_id()] << std::endl;
		break;
	}
}

void Game::processEnemy(mud::enemy& enemy)
{
	auto visiblesTilesIds = seeAround(enemy.tile_id(), 2);
	for (auto tileField : visiblesTilesIds)
	{
		mud::tile tile = tiles[tileField.first];
		if (tile.occupant_type() == mud::tile::CHARACTER)
		{
			if (enemy.facing().value() == tileField.second)
			{
				enemyMoveOrAttack(enemy);
			}
			else
			{
				mud::direction dir;
				dir.set_value(turnLeft(enemy.facing().value()));
				if (dir.value() == tileField.second)
				{
					*enemy.mutable_facing() = dir;
				}
				else 
				{
					dir.set_value(turnRight(enemy.facing().value()));
					*enemy.mutable_facing() = dir;
				}
				std::cout << enemy.name() << " turn to " << getDirection(enemy.facing()) << std::endl;
			}
			break;
		}
	}
}

bool Game::movingCharacter(std::int64_t characterId, std::int64_t tileId)
{
	mud::tile& tile = tiles[tileId];
	if (tile.occupant_type() == mud::tile::NOBODY)
	{
		mud::tile& currentTile = tiles[characters[characterId].tile_id()];
		currentTile.set_occupant_id(0);
		currentTile.set_occupant_type(mud::tile::NOBODY);
		tile.set_occupant_id(characterId);
		tile.set_occupant_type(mud::tile::CHARACTER);
		characters[characterId].set_tile_id(tile.id());

		return true;
	}

	return false;
}

void Game::enemyMoveOrAttack(mud::enemy& enemy)
{
	mud::tile& enemyTile = tiles[enemy.tile_id()];
	std::int64_t facingTileId = 0;

	for (auto& neighbour : enemyTile.neighbours())
	{
		if (neighbour.neighbour_direction().value() == enemy.facing().value())
		{
			facingTileId = neighbour.neighbour_tile_id();
			break;
		}
	}

	if (facingTileId > 0)
	{
		mud::tile& facingTile = tiles[facingTileId];
		if (facingTile.occupant_type() == mud::tile::NOBODY)
		{
			enemyTile.set_occupant_id(0);
			enemyTile.set_occupant_type(mud::tile::NOBODY);
			facingTile.set_occupant_id(enemy.id());
			facingTile.set_occupant_type(mud::tile::ENEMY);
			enemy.set_tile_id(facingTileId);

			std::cout << enemy.name() << " move to " << facingTileId << std::endl;
		}
		else if (facingTile.occupant_type() == mud::tile::CHARACTER)
		{
			mud::character& character = characters[facingTile.occupant_id()];
			std::cout << "\a" << std::flush;
			std::cout << enemy.name() << " attack " << character.name() << std::endl;

			auto& characterLife = getAttribute(character, mud::attribute::LIFE);
			auto& enemyStrength = getAttribute(enemy, mud::attribute::STRENGTH);
			hit(characterLife, enemyStrength.value());

			std::cout << "life: " << characterLife.value() << std::endl;
		}
	}
	else 
	{
		std::cerr << "ERROR: Enemy " << enemy.id() << " try to move nowhere." << std::endl;
	}

}

void Game::hit(mud::attribute& lifeAttribute, std::int64_t damages)
{
	lifeAttribute.set_value(lifeAttribute.value() - damages);
	if (lifeAttribute.value() < 0)
	{
		lifeAttribute.set_value(0);
	}
}

void Game::deleteEnemy(std::int64_t enemyId)
{
	auto& enemy = enemies[enemyId];
	auto& tile = tiles[enemy.tile_id()];
	tile.set_occupant_id(0);
	tile.set_occupant_type(mud::tile::NOBODY);
	enemies.erase(enemyId);
}

std::map<std::int64_t, mud::direction_direction_enum> Game::seeAround(const std::int64_t& currentTileId, const int range)
{
	std::map<std::int64_t, mud::direction_direction_enum> visibleTilesIds;

	if (range > 0) {
		for (auto& neighbour : tiles[currentTileId].neighbours())
		{
			visibleTilesIds.insert({ neighbour.neighbour_tile_id(), neighbour.neighbour_direction().value() });
			
			auto neighbourTiles = seeAround(neighbour.neighbour_tile_id(), range - 1);
			for (auto& t : neighbourTiles)
			{
				if (t.first != currentTileId)
				{
					visibleTilesIds.insert({ t.first, neighbour.neighbour_direction().value() });
				}
			}
		}
	}

	return visibleTilesIds;
}

mud::tile_book Game::getTileBook()
{
	mud::tile_book book{};
	for (auto& field : tiles)
	{
		*book.add_tiles() = field.second;
	}
	return book;
}

mud::enemy_book Game::getEnemyBook()
{
	mud::enemy_book book{};
	for (auto& field : enemies)
	{
		*book.add_enemies() = field.second;
	}
	return book;
}

mud::character_book Game::getCharacterBook()
{
	mud::character_book book{};
	for (auto& field : characters)
	{
		*book.add_characters() = field.second;
	}
	return book;
}
