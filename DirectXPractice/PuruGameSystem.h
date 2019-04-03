#pragma once

#define WIN32_LEAN_AND_MEAN //This excludes certain lesser used APIs
#include<windows.h>

#include"InputClass.h"
#include"GraphicsClass.h"

class PuruGameSystem
{
public:
	PuruGameSystem();
	PuruGameSystem(const PuruGameSystem&);
	~PuruGameSystem();

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

private:
	LPCWSTR m_applicationName;
	HINSTANCE m_hInstance;
	HWND m_hwnd;

	InputClass* m_Input;
	GraphicsClass* m_Graphics;
};

//The WndProc and ApplicationHandle are included so that the built in windows system messaging can be redirected to our own MessageHandler
//Function Prototypes

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//Globals
static PuruGameSystem* ApplicationHandle = nullptr;

