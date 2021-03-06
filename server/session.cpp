#include "session.h"

Session::Session(const std::shared_ptr<std::mutex> m, const std::shared_ptr<Game> g, const std::chrono::duration<double> timeout) : mutex_(m), game_(g), timeout_(timeout)
{

}

grpc::Status Session::GetToken(grpc::ServerContext* context, const mud::token_in* request, mud::token_out* response)
{
	std::int64_t token = 0;
	do {
		token = GenerateNewToken();
	} while (token_states_.find(token) != token_states_.end());
	state state_present{};
	state_present.time_ = std::chrono::system_clock::now();
	state_present.status_ = state::status::TOKEN;
	token_states_.insert({ token,  state_present });
	response->set_token(token);
	return grpc::Status::OK;

}

grpc::Status Session::Login(grpc::ServerContext* context, const mud::login_in* request, mud::login_out* response)
{
	if (token_states_.find(request->token()) != token_states_.end() &&
		token_states_[request->token()].status_ == state::status::TOKEN) {
		mud::player player = game_->GetPlayer(request->name());
		Hash hash(player.passwordhash() + ":" + std::to_string(request->token()));

		if (hash.GetStringHash() != request->hash())
		{
			std::cout << "login failed" << std::endl;
			response->set_login_state(mud::login_out::FAILED);
			return grpc::Status::OK;
		}

		std::cout << request->name() << " is logged" << std::endl;
		response->set_login_state(mud::login_out::OK);
		token_states_[request->token()].status_ = state::status::LOGIN;
		token_states_[request->token()].id_player_ = player.id();
		for (const auto& field : player.idcharacters())
		{
			*response->add_characters() = game_->characters[field];
		}
	}

	return grpc::Status::OK;
}

grpc::Status Session::SelectCharacter(grpc::ServerContext* context, const mud::select_character_in* request, mud::select_character_out* response)
{
	if (token_states_.find(request->token()) != token_states_.end() &&
		token_states_[request->token()].status_ == state::status::LOGIN) {
		mud::player player = game_->players[token_states_[request->token()].id_player_];
		if (request->action() == mud::select_character_in::CREATE)
		{
			bool characterFound = false;
			for (const auto& field : game_->characters)
			{
				if (field.second.name() == request->name())
				{
					characterFound = true;
					break;
				}
			}
			if (characterFound)
			{
				response->set_result(mud::select_character_out::NAME_ALREADY_EXIST);
			}
			else
			{
				mutex_->lock();
				mud::character character = game_->CreateCharacter(request->name(), player.id());
				mutex_->unlock();
				response->set_result(mud::select_character_out::OK);
				*response->mutable_selected_character() = character;
				token_states_[request->token()].id_character_ = character.id();
				token_states_[request->token()].status_ = state::status::PLAYING;

				std::cout << player.name() << " play with the new character " << character.name() << std::endl;
			}
		}
		else if (request->action() == mud::select_character_in::SELECT)
		{
			mud::character character;
			bool characterFound = false;
			for (const auto& field : player.idcharacters())
			{
				if (request->name() == game_->characters[field].name())
				{
					character = game_->characters[field];
					characterFound = true;
					break;
				}
			}
			if (characterFound)
			{
				mutex_->lock();
				bool connected = game_->ConnectCharacter(character.id());
				mutex_->unlock();

				if (connected)
				{
					response->set_result(mud::select_character_out::OK);
					*response->mutable_selected_character() = character;
					token_states_[request->token()].id_character_ = character.id();
					token_states_[request->token()].status_ = state::status::PLAYING;

					std::cout << player.name() << " play with " << character.name() << std::endl;
				}
				else
				{
					response->set_result(mud::select_character_out::TILE_OCCUPED);
					std::cout << player.name() << " want to play with " << character.name() << std::endl;
				}
				
			}
			else
			{
				response->set_result(mud::select_character_out::NOT_FOUND);
			}
		}
	}

	return grpc::Status::OK;
}

grpc::Status Session::Play(grpc::ServerContext* context, const mud::play_in* request, mud::play_out* response)
{
	if (token_states_.find(request->token()) != token_states_.end() &&
		token_states_[request->token()].status_ == state::status::PLAYING) {

		token_states_[request->token()].time_ = std::chrono::system_clock::now();

		game_->charactersInputs[request->character_id()] = request->command();

		if (game_->connectedCharacters.find(request->character_id()) == game_->connectedCharacters.end())
		{
			response->set_status(mud::play_out::FAILURE);
		}
		else
		{
			mud::character& character = *game_->connectedCharacters[request->character_id()];
			response->set_character_id(request->character_id());

			if (getAttribute(character, mud::attribute::LIFE).value() <= 0)
			{
				response->set_status(mud::play_out::DEAD);
			}
			else
			{
				response->set_status(mud::play_out::SUCCESS);
			}


			*response->add_tiles() = game_->tiles[character.tile_id()];
			*response->add_characters() = character;
			for (const auto& field : game_->seeAround(character.tile_id(), 2))
			{
				mud::tile tile = game_->tiles[field.first];
				*response->add_tiles() = tile;

				if (tile.occupant_type() == mud::tile::CHARACTER)
				{
					*response->add_characters() = *game_->connectedCharacters[tile.occupant_id()];
				}
				else if (tile.occupant_type() == mud::tile::ENEMY)
				{
					*response->add_enemies() = game_->enemies[tile.occupant_id()];
				}
			}

		}

	}
	return grpc::Status::OK;
}

grpc::Status Session::Logout(grpc::ServerContext* context, const mud::logout_in* request, mud::logout_out* response)
{
	if (token_states_.find(request->token()) != token_states_.end())
	{
		if (token_states_[request->token()].id_character_ != 0)
		{
			game_->DisconnectCharacter(token_states_[request->token()].id_character_);
		}
		token_states_.erase(token_states_.find(request->token()));
	}
	return grpc::Status::OK;
}

static std::int64_t nextToken = 42;
std::int64_t Session::GenerateNewToken()
{
	return nextToken++;
}
