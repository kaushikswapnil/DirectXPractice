#include"PuruGameSystem.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	PuruGameSystem* system;
	bool result;

	//Create system object
	system = new PuruGameSystem();

	if(!system)
	{
		return 0;
	}

	//Initialize and run system object

	result = system->Initialize();
	if (result)
	{
		system->Run();
	}

	//Shutdown and release system object
	system->Shutdown();
	delete system;
	system = nullptr;

	return 0;
}