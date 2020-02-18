#include <iostream>
#include <fstream>
#include "game.h"
#include "session.h"
#include <grpcpp/server_builder.h>
#include <grpcpp/security/credentials.h>
#include <mutex>
#include <thread>

bool registerPlayer(mud::player_book &book)
{
	std::int64_t maxId = 0;
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
		return false;
	}
	return true;
}

void showPlayers()
{
	mud::player_book book{};

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

	auto pg = std::make_shared<Game>();
	auto m = std::make_shared<std::mutex>();
	Session session{ m, pg };
	std::string server_address{ "0.0.0.0:4242" };
	grpc::ServerBuilder builder{};

	builder.AddListeningPort(
		server_address,
		grpc::InsecureServerCredentials());

	builder.RegisterService(&session);
	std::unique_ptr<grpc::Server> server(builder.BuildAndStart());


	std::thread t([&server]
	{
		server->Wait();
	});


	bool loop = true;
	auto totalTime = std::chrono::milliseconds(1000);
	while (loop)
	{
		auto startTime = std::chrono::high_resolution_clock::now();
		m->lock();
		loop = pg->runOnce();
		m->unlock();
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

	t.join();

	return 0;
}
