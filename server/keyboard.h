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

	enum class input_t {
		NONE = 0,
		FORWARD = 1,
		BACKWARD = 2,
		LEFT = 3,
		RIGHT = 4,
		ATTACK = 5,
		QUIT = 6,
		INFO = 7,
		PRINT = 8,
	};

	class Keyboard
	{
	public:
		const std::map<input_t, int> input_key =
		{
			{ input_t::ATTACK,   VK_RETURN },
			{ input_t::BACKWARD, VK_DOWN },
			{ input_t::FORWARD,  VK_UP },
			{ input_t::INFO,     VK_TAB },
			{ input_t::LEFT,     VK_LEFT },
			{ input_t::RIGHT,    VK_RIGHT },
			{ input_t::PRINT,    VK_SNAPSHOT },
			{ input_t::QUIT,     VK_ESCAPE }
		};
	public:
		void stop();
		void run();
		bool check_released_input(const input_t& key);
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
		std::map<input_t, bool> key_released_ = {};
		std::array<key_state, 256> key_state_inputs_ = {};
		std::array<short, 256> new_key_states_ = { 0 };
		std::array<short, 256> old_key_states_ = { 0 };
	};


std::ostream& operator<< (std::ostream& os, const input_t& key);