#pragma once
#include <fstream>
#include <string>
#include <vector>
// This has to change to a more portable way.
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include "../mud_lib/helper.h"
#include <chrono>
#include <ctime> 
#include <iostream>

class Game 
{
public:
	Game();
	bool runOnce();
	mud::player GetPlayer(std::string name);
	mud::character CreateCharacter(std::string name, std::int64_t playerId);
	bool ConnectCharacter(std::int64_t characterId);
	void DisconnectCharacter(std::int64_t characterId);
	std::map<std::int64_t, mud::direction_direction_enum> seeAround(
		const std::int64_t& currentTileId,
		const int range = 2);

	std::map<std::int64_t, mud::character> characters;
	std::map<std::int64_t, mud::player> players;
	std::map<std::int64_t, mud::play_in::input_enum> charactersInputs;
	std::map<std::int64_t, mud::tile> tiles;
	std::map<std::int64_t, mud::enemy> enemies;
	std::map<std::int64_t, mud::character*> connectedCharacters;
	std::int64_t iteration = 0;
protected:
	void initMap();
	mud::tile createTile(mud::tile_tile_type type, std::string mood);
	void addNeighbour(mud::tile& tile, mud::tile& neighbour, mud::direction_direction_enum direction);
	void loadData();
	bool saveData();
	mud::direction_direction_enum Game::getOppositeDirection(mud::direction_direction_enum dir);
	void showCharacters();
	bool createCharacter();
	void processCharacter(std::int64_t characterId);
	void processEnemy(mud::enemy& enemy);
	bool movingCharacter(std::int64_t characterId, std::int64_t tileId);
	void enemyMoveOrAttack(mud::enemy& enemy);
	void hit(mud::attribute& lifeAttribute, std::int64_t damages);
	void deleteEnemy(std::int64_t enemyId);
	void characterDead(std::int64_t characterId);

	

	mud::tile_book getTileBook();
	mud::enemy_book getEnemyBook();
	mud::character_book getCharacterBook();
	mud::player_book getPlayerBook();
};

