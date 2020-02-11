#include "client.h"
#include "../mud_lib/hash.h"
#include <grpcpp/create_channel.h>

int main(int ac, char** av) 
{
	Client c(
		grpc::CreateChannel(
			//"192.168.7.74:4242",
			"localhost:4242",
			grpc::InsecureChannelCredentials()));
	auto token = std::make_shared<std::int64_t>(0);
	auto status = c.GetToken(token);
	std::cout << *token << std::endl;

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

		status = c.Login(name, publicHash.GetStringHash(), *token, lo);

		if (lo->login_state() == mud::login_out::OK) 
		{
			loop = false;
		}
		else
		{
			std::cout << "Login failed." << std::endl;
		}
	}


	loop = true;
	std::string characterName;
	mud::character character;
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
				c.CreateCharacter(newCharacterName, *token, sco);
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
			for (auto& field : lo->characters())
			{
				if (field.name() == characterName)
				{
					character = field;
					c.SelectCharacter(characterName, *token);

					loop = false;
					break;
				}
			}
		}
	}

	system("pause");
	return 0;
}