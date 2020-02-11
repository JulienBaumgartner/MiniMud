#include "client.h"

Client::Client(std::shared_ptr<grpc::Channel> channel) : stub_(mud::game_server::NewStub(channel))
{
}

grpc::Status Client::GetToken(std::shared_ptr<std::int64_t> value)
{
	grpc::ClientContext client_context;
	mud::token_in ti{};
	mud::token_out to{};
	auto status = stub_->GetToken(&client_context, ti, &to);
	if (!status.ok())
	{
		throw std::runtime_error("couldn't get anything!");
	}
	token_ = to.token();
	*value = token_;
	return status;

}

grpc::Status Client::Login(std::string name, std::string hash, std::int64_t token, std::shared_ptr<mud::login_out> value)
{
	grpc::ClientContext client_context;
	mud::login_in li{};
	mud::login_out lo{};
	li.set_name(name);
	li.set_hash(hash);
	li.set_token(token);
	auto status = stub_->Login(&client_context, li, &lo);
	if (!status.ok())
	{
		throw std::runtime_error("couldn't login!");
	}
	if (lo.login_state() == mud::login_out::OK)
	{
		std::cout << "Characters:" << std::endl;

		for (const auto& field : lo.characters())
		{
			std::cout << field << std::endl;
		}
	}
	*value = lo;
	return grpc::Status::OK;
}

grpc::Status Client::SelectCharacter(std::string name, std::int64_t token)
{
	grpc::ClientContext client_context;
	mud::select_character_in sci{};
	mud::select_character_out sco{};
	sci.set_action(mud::select_character_in::SELECT);
	sci.set_name(name);
	sci.set_token(token);
	auto status = stub_->SelectCharacter(&client_context, sci, &sco);
	if (!status.ok())
	{
		throw std::runtime_error("couldn't load character!");
	}

	return grpc::Status::OK;
}

grpc::Status Client::CreateCharacter(std::string name, std::int64_t token, std::shared_ptr<mud::select_character_out> value)
{
	grpc::ClientContext client_context;
	mud::select_character_in sci{};
	mud::select_character_out sco{};
	sci.set_action(mud::select_character_in::CREATE);
	sci.set_name(name);
	sci.set_token(token);
	auto status = stub_->SelectCharacter(&client_context, sci, &sco);
	if (!status.ok())
	{
		throw std::runtime_error("couldn't create character!");
	}
	*value = sco;
	return grpc::Status::OK;
}
