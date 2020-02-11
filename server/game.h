#pragma once
#include <fstream>
#include <string>
#include <vector>
#include "../mud_lib/helper.h"
#include "../mud_lib/keyboard.h"

class Game 
{
public:
	Game();
	void run();
	mud::player GetPlayer(std::string name);
	mud::character CreateCharacter(std::string name, std::int64_t playerId);

	std::map<std::int64_t, mud::character> characters;
	std::map<std::int64_t, mud::player> players;
protected:
	input_t execute_keyboard();
	void initMap();
	mud::tile createTile(mud::tile_tile_type type, std::string mood);
	void addNeighbour(mud::tile& tile, mud::tile& neighbour, mud::direction_direction_enum direction);
	void loadData();
	bool saveData();
	mud::direction_direction_enum Game::getOppositeDirection(mud::direction_direction_enum dir);
	void showCharacters();
	bool createCharacter();
	void processCharacter(input_t input);
	void processEnemy(mud::enemy& enemy);
	bool movingCharacter(std::int64_t characterId, std::int64_t tileId);
	void enemyMoveOrAttack(mud::enemy& enemy);
	void hit(mud::attribute& lifeAttribute, std::int64_t damages);
	void deleteEnemy(std::int64_t enemyId);
	std::map<std::int64_t, mud::direction_direction_enum> seeAround(
		const std::int64_t& currentTileId,
		const int range = 2);



	mud::tile_book getTileBook();
	mud::enemy_book getEnemyBook();
	mud::character_book getCharacterBook();
	mud::player_book getPlayerBook();

	std::int64_t characterId;
	Keyboard keyboard;

	std::map<std::int64_t, mud::tile> tiles;
	std::map<std::int64_t, mud::enemy> enemies;
};

