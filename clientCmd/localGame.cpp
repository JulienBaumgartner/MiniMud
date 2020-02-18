#include "localGame.h"

LocalGame::LocalGame():
	client(grpc::CreateChannel(
	"localhost:4242",
	grpc::InsecureChannelCredentials()))
{
	
	token = std::make_shared<std::int64_t>(0);
	auto status = client.GetToken(token);
}

void LocalGame::run()
{
	auto lo = login();
	selectCharacter(lo);

	keyboard.run();

	bool loop = true;
	auto totalTime = std::chrono::milliseconds(1000);
	while (loop)
	{
		auto startTime = std::chrono::high_resolution_clock::now();
		mud::play_in::input_enum input = execute_keyboard();
		if (input == mud::play_in::QUIT)
		{
			client.Logout(*token, playerId, character.id());
			loop = false;
		}
		else
		{

			auto po = std::make_shared<mud::play_out>();
			auto& status = client.Play(*token, playerId, character.id(), input, po);

			tiles.clear();
			for (const auto& field : po->tiles())
			{
				tiles.insert({ field.id(), field });
			}
			characters.clear();
			for (const auto& field : po->characters())
			{
				characters.insert({ field.id(), field });
				if (character.id() == field.id())
				{
					character = field;
				}
			}
			enemies.clear();
			for (const auto& field : po->enemies())
			{
				enemies.insert({ field.id(), field });
			}

			showMap();
		}

		auto endTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> duration = endTime - startTime;
		if (totalTime > duration)
		{
			std::this_thread::sleep_for(totalTime - duration);
		}
		else
		{
			std::cerr << "too long..." << std::endl;
		}
	}

	keyboard.stop();
}

mud::login_out LocalGame::login()
{
	std::string name;
	std::string password;
	bool loop = true;

	auto lo = std::make_shared<mud::login_out>();
	while (loop)
	{
		std::cout << "Enter your name: ";
		std::cin >> name;
		std::cout << "Enter your password: ";
		std::cin >> password;

		Hash privateHash(name + ":" + password);
		Hash publicHash(privateHash.GetStringHash() + ":" + std::to_string(*token));

		auto status = client.Login(name, publicHash.GetStringHash(), *token, lo);

		if (lo->login_state() == mud::login_out::OK)
		{
			loop = false;
		}
		else
		{
			std::cout << "Login failed." << std::endl;
		}
	}

	return *lo;
}

void LocalGame::selectCharacter(mud::login_out& lo)
{
	bool loop = true;
	std::string characterName;
	auto sco = std::make_shared<mud::select_character_out>();
	std::cout << "Select your character or type \"new\":" << std::endl;
	while (loop)
	{
		std::cin >> characterName;
		if (characterName == "new")
		{
			bool loop2 = true;
			while (loop2)
			{
				std::cout << "Enter your new character name:";
				std::string newCharacterName;
				std::cin >> newCharacterName;
				client.CreateCharacter(newCharacterName, *token, sco);
				if (sco->result() == mud::select_character_out::NAME_ALREADY_EXIST)
				{
					std::cout << "This character already exist!" << std::endl;
				}
				else if (sco->result() == mud::select_character_out::OK)
				{
					character = sco->selected_character();
					loop = false;
					loop2 = false;
				}
			}
		}
		else
		{
			for (auto& field : lo.characters())
			{
				if (field.name() == characterName)
				{
					character = field;
					client.SelectCharacter(characterName, *token);

					loop = false;
					break;
				}
			}
		}
	}
}

mud::play_in::input_enum LocalGame::execute_keyboard()
{
	mud::play_in::input_enum entry = mud::play_in::NONE;

	for (const auto& field : keyboard.input_key)
	{
		if (keyboard.check_released_input(field.first))
		{
			return field.first;
		}
	}
	return entry;
}

void LocalGame::showMap()
{
	mud::tile currentTile = tiles[character.tile_id()];

	std::cout << "-----------" << std::endl;

	std::int64_t northTileId = getNeighbourId(currentTile, mud::direction::NORTH);
	if (northTileId != 0)
	{
		mud::tile northTile = tiles[northTileId];
		std::cout << "|   ";
		showTile(northTile);
		std::cout << "   |" << std::endl;
	}
	else
	{
		std::cout << "|         |" << std::endl;
	}

	std::cout << "|";
	std::int64_t westTileId = getNeighbourId(currentTile, mud::direction::WEST);
	if (westTileId != 0)
	{
		mud::tile westTile = tiles[westTileId];
		showTile(westTile);
	}
	else
	{
		std::cout << "   ";
	}
	showCurrentTile(currentTile);
	std::int64_t eastTileId = getNeighbourId(currentTile, mud::direction::EAST);
	if (eastTileId != 0)
	{
		mud::tile eastTile = tiles[eastTileId];
		showTile(eastTile);
	}
	else 
	{
		std::cout << "   ";
	}
	std::cout << "|" << std::endl;


	std::int64_t southTileId = getNeighbourId(currentTile, mud::direction::SOUTH);
	if (southTileId != 0)
	{
		mud::tile southTile = tiles[southTileId];
		std::cout << "|   ";
		showTile(southTile);
		std::cout << "   |" << std::endl;
	}
	else
	{
		std::cout << "|         |" << std::endl;
	}
	std::cout << "-----------" << std::endl;
}

void LocalGame::showTile(const mud::tile& tile)
{
	char occupant = ' ';
	if (tile.occupant_type() == mud::tile::CHARACTER)
	{
		occupant = 'c';
	}
	else if (tile.occupant_type() == mud::tile::ENEMY)
	{
		occupant = 'e';
	}
	std::cout << "[" << occupant << "]";
}

void LocalGame::showCurrentTile(const mud::tile& tile)
{
	char occupant = ' ';
	if (character.facing().value() == mud::direction::NORTH)
	{
		occupant = '^';
	}
	else if (character.facing().value() == mud::direction::WEST)
	{
		occupant = '<';
	}
	else if (character.facing().value() == mud::direction::EAST)
	{
		occupant = '>';
	}
	else if (character.facing().value() == mud::direction::SOUTH)
	{
		occupant = 'v';
	}
	std::cout << "[" << occupant << "]";
}
