#include "../protobuf_mud_lib/mud_lib.grpc.pb.h"
#include <grpcpp/channel.h>
#include "../mud_lib/helper.h"

class Client
{
public:
	Client(std::shared_ptr<grpc::Channel> channel);
	grpc::Status GetToken(std::shared_ptr<std::int64_t> value);
	grpc::Status Login(std::string name, std::string hash, std::int64_t token, std::shared_ptr<mud::login_out> lo);
	grpc::Status SelectCharacter(std::string name, std::int64_t token);
	grpc::Status CreateCharacter(std::string name, std::int64_t token, std::shared_ptr<mud::select_character_out> sco);
private:
	std::int64_t token_ = 0;
	std::unique_ptr<mud::game_server::Stub> stub_;
};
