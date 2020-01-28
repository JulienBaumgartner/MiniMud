#pragma once
#include <fstream>
#include <string>
#include "helper.h"
#include "keyboard.h"

class Game 
{
public:
	void run();
protected:
	void initMap();
	mud::tile createTile(mud::tile_type type, std::string mood);
	void addNeighbour(mud::tile& tile, mud::tile& neighbour, mud::direction direction);
	void loadData();
	bool saveData();
	bool saveTiles();
	bool saveEnemies();
	bool saveCharacters();
	mud::direction getOppositeDirection(mud::direction dir);
	void showCharacters();
	bool createCharacter();
	void processCharacter(Keyboard::Input input);
	bool movingCharacter(mud::character character, mud::tile tile);

	std::int64_t characterId;

	std::map<std::int64_t, mud::tile> tiles;
	std::map<std::int64_t, mud::enemy> enemies;
	std::map<std::int64_t, mud::character> characters;


};

