#pragma once

#include <mutex>
#include <thread>
#include <array>
#include "helper.h"
// This has to change to a more portable way.
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

	class Keyboard
	{
	public:
		const std::map<mud::play_in::input_enum, int> input_key =
		{
			{ mud::play_in::ATTACK,   VK_RETURN },
			{ mud::play_in::BACKWARD, VK_DOWN },
			{ mud::play_in::FORWARD,  VK_UP },
			{ mud::play_in::INFO,     VK_TAB },
			{ mud::play_in::LEFT,     VK_LEFT },
			{ mud::play_in::RIGHT,    VK_RIGHT },
			{ mud::play_in::PRINT,    VK_SNAPSHOT },
			{ mud::play_in::QUIT,     VK_ESCAPE }
		};
	public:
		void stop();
		void run();
		bool check_released_input(const mud::play_in::input_enum& key);
	protected:
		struct key_state
		{
			bool pressed_ = false;
			bool released_ = false;
			bool held_ = false;
		};
		std::thread backgroud_thread_;
		std::mutex mutex_;
		bool running_ = false;
		std::map<mud::play_in::input_enum, bool> key_released_ = {};
		std::array<key_state, 256> key_state_inputs_ = {};
		std::array<short, 256> new_key_states_ = { 0 };
		std::array<short, 256> old_key_states_ = { 0 };
	};


std::ostream& operator<< (std::ostream& os, const mud::play_in::input_enum& key);