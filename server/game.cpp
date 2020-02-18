#include "game.h"

Game::Game()
{
	loadData();

	if (tiles.size() == 0)
	{
		initMap();
		saveBook("tiles.json", getTileBook());
	}
}

bool Game::runOnce()
{
	bool loop = true;

	for (auto& character : connectedCharacters)
	{
		processCharacter(character.first);
	}

	for (auto& enemy : enemies)
	{
		processEnemy(enemy.second);
	}

	for (auto& character : connectedCharacters)
	{
		auto& characterLife = getAttribute(*character.second, mud::attribute::LIFE);
		if (characterLife.value() == 0)
		{
			characterDead(character.first);
		}
	}

	iteration++;
	return loop;
}

mud::player Game::GetPlayer(std::string name)
{
	for (const auto& field : players)
	{
		if (field.second.name() == name)
		{
			return field.second;
		}
	}
	return mud::player();
}

mud::character Game::CreateCharacter(std::string name, std::int64_t playerId)
{
	std::int64_t maxId = 0;
	for (const auto& field : characters)
	{
		if (field.first > maxId) {
			maxId = field.first;
		}
	}

	mud::character character{};
	character.set_name(name);
	
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


	characters.insert({ character.id(), character });
	connectedCharacters.insert({ character.id(), &character });

	mud::player& player = players[playerId];
	player.add_idcharacters(character.id());

	saveData();

	mud::tile& tile = tiles[character.tile_id()];
	tile.set_occupant_type(mud::tile::CHARACTER);
	tile.set_occupant_id(character.id());
	return character;
}

bool Game::ConnectCharacter(std::int64_t characterId)
{
	mud::character& character = characters[characterId];
	mud::tile& tile = tiles[character.tile_id()];
	if (tile.occupant_type() == mud::tile::EMPTY)
	{
		tile.set_occupant_type(mud::tile::CHARACTER);
		tile.set_occupant_id(characterId);
		connectedCharacters[characterId] = &character;
		return true;
	}

	return false;
}

void Game::DisconnectCharacter(std::int64_t characterId)
{
	mud::character& character = *connectedCharacters[characterId];
	mud::tile& tile = tiles[character.tile_id()];
	tile.set_occupant_type(mud::tile::NOBODY);
	tile.set_occupant_id(0);
	std::cout << character.name() << " leave the game." << std::endl;
	connectedCharacters.erase(connectedCharacters.find(characterId));
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
	mud::player_book playerBook{};

	loadBook("tiles.json", tilesBook);
	loadBook("enemies.json", enemyBook);
	loadBook("characters.json", characterBook);
	loadBook("players.json", playerBook);

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
	for (const auto& p : playerBook.players())
	{
		players.insert({ p.id(), p });
	}
}

bool Game::saveData()
{
	bool flag = true;
	flag = saveBook("tiles.json", getTileBook()) && flag;
	flag = saveBook("enemies.json", getEnemyBook()) && flag;
	flag = saveBook("characters.json", getCharacterBook()) && flag;
	flag = saveBook("players.json", getPlayerBook()) && flag;

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
	std::int64_t maxId = 0;
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

	characters.insert({ character.id(), character });

	std::cout << "Adding character:" << std::endl;
	std::cout << character << std::endl;

	return saveBook("characters.json", getCharacterBook());
}

void Game::processCharacter(std::int64_t characterId)
{
	mud::character& character = *connectedCharacters[characterId];
	mud::tile& tile = tiles[character.tile_id()];
	mud::tile destination;
	mud::direction direction{};
	std::int64_t facingTileId = 0;
	bool foundDestination = false;

	switch (charactersInputs[characterId])
	{
	case mud::play_in::ATTACK:
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
	case mud::play_in::FORWARD:

		for (const auto& neighbour : tile.neighbours()) {
			if (neighbour.neighbour_direction().value() == character.facing().value())
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
			std::cout << "No tile forward you " << getDirection(character.facing()) << std::endl;
		}
		break;

	case mud::play_in::LEFT:
		direction.set_value(turnLeft(character.facing().value()));
		*character.mutable_facing() = direction;
		std::cout << "You face " << getDirection(direction) << std::endl;
		break;

	case mud::play_in::RIGHT:
		direction.set_value(turnRight(character.facing().value()));
		*character.mutable_facing() = direction;
		std::cout << "You face " << getDirection(direction) << std::endl;
		break;

	case mud::play_in::INFO:
		std::cout << std::endl << "Character:" << std::endl;
		std::cout << character << std::endl;
		std::cout << std::endl << "Tile:" << std::endl;
		std::cout << tiles[character.tile_id()] << std::endl;
		break;
	}
}

void Game::processEnemy(mud::enemy& enemy)
{
	auto visiblesTilesIds = seeAround(enemy.tile_id(), 1);
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
		mud::tile& currentTile = tiles[connectedCharacters[characterId]->tile_id()];
		currentTile.set_occupant_id(0);
		currentTile.set_occupant_type(mud::tile::NOBODY);
		tile.set_occupant_id(characterId);
		tile.set_occupant_type(mud::tile::CHARACTER);
		connectedCharacters[characterId]->set_tile_id(tile.id());

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
			mud::character& character = *connectedCharacters[facingTile.occupant_id()];
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

void Game::characterDead(std::int64_t characterId)
{

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

mud::player_book Game::getPlayerBook()
{
	mud::player_book book{};
	for (auto& field : players)
	{
		*book.add_players() = field.second;
	}
	return book;
}
