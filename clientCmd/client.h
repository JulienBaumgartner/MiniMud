#include "../mud_lib/helper.h"

class Client
{
public:
	Client(std::shared_ptr<grpc::Channel> channel);
	grpc::Status GetToken(std::shared_ptr<std::int64_t> value);
	grpc::Status Login(std::string name, std::string hash, std::int64_t token, std::shared_ptr<mud::login_out> lo);
	grpc::Status SelectCharacter(std::string name, std::int64_t token);
	grpc::Status CreateCharacter(std::string name, std::int64_t token, std::shared_ptr<mud::select_character_out> sco);
	grpc::Status Play(std::int64_t token, std::int64_t playerId, std::int64_t characterId,
					  mud::play_in::input_enum& command, std::shared_ptr<mud::play_out> po);
	grpc::Status Logout(std::int64_t token, std::int64_t playerId, std::int64_t characterId);
private:
	std::int64_t token_ = 0;
	std::unique_ptr<mud::game_server::Stub> stub_;
};
