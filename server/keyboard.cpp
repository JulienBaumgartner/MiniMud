#include "keyboard.h"
Keyboard::Input Keyboard::GetInput()
{
	Keyboard::Input input;

	if (GetAsyncKeyState(VK_UP) < 0) 
	{
		std::cout << "UP" << std::endl;
		return Keyboard::Forward;
	}
	if (GetAsyncKeyState(VK_DOWN) < 0)
	{
		std::cout << "DOWN" << std::endl;
		return Keyboard::Backward;
	}
	if (GetAsyncKeyState(VK_RIGHT) < 0)
	{
		std::cout << "RIGHT" << std::endl;
		return Keyboard::TurnRight;
	}
	if (GetAsyncKeyState(VK_LEFT) < 0)
	{
		std::cout << "LEFT" << std::endl;
		return Keyboard::TurnLeft;
	}
	if (GetAsyncKeyState(VK_RETURN) < 0)
	{
		std::cout << "ENTER" << std::endl;
		return Keyboard::Attack;
	}
	if (GetAsyncKeyState(VK_ESCAPE) < 0)
	{
		std::cout << "ESC" << std::endl;
		return Keyboard::Quit;
	}
	if (GetAsyncKeyState(VK_TAB) < 0)
	{
		std::cout << "TAB" << std::endl;
		return Keyboard::Info;
	}

	return Keyboard::None;
}
