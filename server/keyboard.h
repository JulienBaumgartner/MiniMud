#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <iostream>

class Keyboard {
public:
	enum Input 
	{
		None,
		Attack,
		Forward,
		Backward,
		TurnRight,
		TurnLeft,
		Quit,
		Info
	};

	Input GetInput();
};