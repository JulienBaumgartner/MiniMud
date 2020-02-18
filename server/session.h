#include "game.h"

#include "../mud_lib/hash.h"
#include "../mud_lib/helper.h"

class Session final : public mud::game_server::Service {
public:
	Session(
		const std::shared_ptr<std::mutex> m,
		const std::shared_ptr<Game> g,
		const std::chrono::duration<double> timeout =
		std::chrono::minutes(10));

	grpc::Status GetToken(
		grpc::ServerContext* context,
		const mud::token_in* request,
		mud::token_out* response) override;

	grpc::Status Login(
		grpc::ServerContext* context,
		const mud::login_in* request,
		mud::login_out* response) override;

	grpc::Status SelectCharacter(
		grpc::ServerContext* context,
		const mud::select_character_in* request,
		mud::select_character_out* response) override;

	grpc::Status Play(
		grpc::ServerContext* context,
		const mud::play_in* request,
		mud::play_out* response) override;

	grpc::Status Logout(
		grpc::ServerContext* context,
		const mud::logout_in* request,
		mud::logout_out* response) override;

public:
	struct state {
		enum class status {
			TOKEN = 0,
			LOGIN = 1,
			PLAYING = 2
		};
		std::int64_t id_player_;
		std::int64_t id_character_;
		std::chrono::system_clock::time_point time_;
		status status_;
	};

private:
	std::int64_t GenerateNewToken();

	const std::shared_ptr<std::mutex> mutex_;
	const std::shared_ptr<Game> game_;
	std::unordered_map<std::int64_t, state> token_states_;
	std::unordered_map<std::int64_t, std::int64_t>
		player_id_tokens_;
	const std::chrono::duration<double> timeout_;
};
