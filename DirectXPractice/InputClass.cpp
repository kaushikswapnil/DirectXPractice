#include "InputClass.h"



InputClass::InputClass()
{
}

InputClass::InputClass(const InputClass &)
{
}


InputClass::~InputClass()
{
}

void InputClass::Initialize()
{

	//initialize all the keys to being released and not pressed
	for (bool & key : m_keys)
	{
		key = false;
	}

	return;
}

void InputClass::Shutdown()
{
}

void InputClass::KeyDown(unsigned int input)
{
	//If key is pressed then save that state in key array
	m_keys[input] = true;
	return;
}

void InputClass::KeyUp(unsigned int input)
{
	//Set key up
	m_keys[input] = false;
	return;
}

bool InputClass::IsKeyDown(unsigned int key)
{
	return m_keys[key];
}
