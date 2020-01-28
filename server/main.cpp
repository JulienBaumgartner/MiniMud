#include <iostream>
#include <fstream>
#include <string>
#include "game.h"
#include "helper.h"

bool registerPlayer(mud::playerBook &book)
{
	int maxId = 0;
	for (const auto& player : book.players())
	{
		if (player.id() > maxId) {
			maxId = player.id();
		}
	}

	mud::player player{};
	std::string name = player.name();
	std::string inputName;
	std::cout << "Enter your name:" << std::endl;
	bool correctName = false;
	while (!correctName)
	{
		std::cin >> inputName;
		player.set_name(inputName);
		correctName = true;
		for (const auto& p : book.players())
		{
			if (player.name() == p.name())
			{
				std::cout << "This player already exist!" << std::endl;
				correctName = false;
				break;
			}
		}
	}
	std::int64_t id = player.id();
	player.set_id(maxId + 1);

	*book.add_players() = player;

	std::cout << "Adding player:" << std::endl;
	std::cout << player << std::endl;


	std::ofstream ofs(
		"player.data",
		std::ios::out | std::ios::binary);
	if (!book.SerializeToOstream(&ofs)) {
		std::cerr << "error writing" << std::endl;
		return -1;
	}
}

void showPlayers()
{
	mud::playerBook book{};

	std::ifstream ifs(
		"player.data",
		std::ios::in | std::ios::binary);
	if (!ifs) {
		std::cerr << "error reading" << std::endl;
	}
	else {
		book.ParseFromIstream(&ifs);
	}

	std::cout << "Players list:" << std::endl;
	for (const auto& player : book.players())
	{
		std::cout << player << std::endl;
	}

	mud::tile tile{};
}

int main(int ac, char** av) {
	std::cout << "starting server!" << std::endl;

	Game game;
	game.run();

	return 0;
}
