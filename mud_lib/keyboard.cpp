#include "keyboard.h"

	void Keyboard::run()
	{
		if (running_) return;
		running_ = true;
		backgroud_thread_ = std::thread([this]
		{
			bool local_running = true;
			do {
				for (int i = 0; i < 256; ++i) {
					new_key_states_[i] = GetAsyncKeyState(i);
					key_state_inputs_[i].pressed_ = false;
					key_state_inputs_[i].released_ = false;
					if (new_key_states_[i] != old_key_states_[i])
					{
						if (new_key_states_[i] & 0x8000)
						{
							key_state_inputs_[i].pressed_ =
								key_state_inputs_[i].held_;
							key_state_inputs_[i].held_ = true;
						}
						else
						{
							key_state_inputs_[i].released_ = true;
							key_state_inputs_[i].held_ = false;
						}
					}
					old_key_states_[i] = new_key_states_[i];
				}
				{
					std::lock_guard l(mutex_);
					local_running = running_;
					for (const auto& key : input_key) {
						if (key_state_inputs_[key.second].released_)
						{
							key_released_[key.first] = true;
						}
					}
				}
			} while (local_running);
		});
	}

	bool Keyboard::check_released_input(const mud::play_in::input_enum& key)
	{
		std::lock_guard l(mutex_);
		if (key_released_[key]) {
			std::cout << key << std::endl;
			key_released_[key] = false;
			return true;
		}
		return false;
	}

	void Keyboard::stop() {
		{
			std::lock_guard l(mutex_);
			running_ = false;
		}
		backgroud_thread_.join();
	}


std::ostream& operator<< (std::ostream& os, const mud::play_in::input_enum& key)
{
	switch (key)
	{
	case mud::play_in::ATTACK:
		os << "ATTACK";
		break;
	case mud::play_in::BACKWARD:
		os << "BACKWARD";
		break;
	case mud::play_in::FORWARD:
		os << "FORWARD";
		break;
	case mud::play_in::INFO:
		os << "INFO";
		break;
	case mud::play_in::LEFT:
		os << "LEFT";
		break;
	case mud::play_in::NONE:
		os << "NONE";
		break;
	case mud::play_in::PRINT:
		os << "PRINT";
		break;
	case mud::play_in::QUIT:
		os << "QUIT";
		break;
	case mud::play_in::RIGHT:
		os << "RIGHT";
		break;
	}
	return os;
}