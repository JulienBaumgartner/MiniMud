#pragma once
#include "../mud_lib/keyboard.h"
#include "client.h"
#include "../mud_lib/hash.h"
#include <string>
#include "../mud_lib/helper.h"

class LocalGame
{
public:
	LocalGame();
	void run();

private:
	mud::login_out login();
	void selectCharacter(mud::login_out& lo);
	mud::play_in::input_enum execute_keyboard();
	void showMap();
	void showTile(const mud::tile& tile);
	void showCurrentTile(const mud::tile& tile);

	Client client;
	std::shared_ptr<std::int64_t> token;
	std::int64_t playerId;
	mud::character character;
	Keyboard keyboard;

	std::map<std::int64_t, mud::tile> tiles;
	std::map<std::int64_t, mud::enemy> enemies;
	std::map<std::int64_t, mud::character> characters;

	std::string ip = "localhost:4242";
};