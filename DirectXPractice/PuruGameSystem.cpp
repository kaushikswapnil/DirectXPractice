#include "PuruGameSystem.h"



PuruGameSystem::PuruGameSystem() : m_Input(nullptr), m_Graphics(nullptr)
{
}

PuruGameSystem::PuruGameSystem(const PuruGameSystem &)
{
}


PuruGameSystem::~PuruGameSystem()
{
}

bool PuruGameSystem::Initialize()
{
	int screenWidth, screenHeight;
	bool result;

	//Initialize the width and height of screen to zero before sending the variables into the function
	screenWidth = screenHeight = 0;

	//initialize the old windows API
	InitializeWindows(screenWidth, screenHeight);

	//Create the input object. This object will be used to handle reading the keyboard input from the user
	m_Input = new InputClass();
	if (!m_Input)
	{
		return false;
	}

	//Initialize input object
	m_Input->Initialize();

	//Create the graphics object to handle all rendering
	m_Graphics = new GraphicsClass();
	if (!m_Graphics)
	{
		return false;
	}

	//initialize the graphics object
	result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);
	if (!result)
	{
		return false;
	}

	return true;
}

void PuruGameSystem::Shutdown()
{
	if (m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = nullptr;
	}

	if (m_Input)
	{
		m_Input->Shutdown();
		delete m_Input;
		m_Input = nullptr;
	}

	//Shutdown windows
	ShutdownWindows();
}

void PuruGameSystem::Run()
{
	MSG msg;
	bool done = false, result;

	//Initialize message structure
	ZeroMemory(&msg, sizeof(MSG));

	//Loop until there is a quit message
	while (!done)
	{
		//handle windows messages
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//If windows signals to end the application then exit out
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			//Frame processing
			result = Frame();
			if (!result)
			{
				done = true;
			}
		}

	}
}

LRESULT PuruGameSystem::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
	case WM_KEYDOWN:
		m_Input->KeyDown((unsigned int)wparam);
		return 0;
		break;

	case WM_KEYUP:
		m_Input->KeyUp((unsigned int)wparam);
		return 0;
		break;

	default:
		return DefWindowProc(hwnd, umsg, wparam, lparam);
		break;
	}
}

bool PuruGameSystem::Frame()
{
	bool result;
	
	//check if user pressed escape
	if (m_Input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	//Do frame processing for Graphics
	result = m_Graphics->Frame();
	if (!result)
	{
		return false;
	}

	return true;
}

void PuruGameSystem::InitializeWindows(int & screenWidth, int & screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	//Get external pointer to this object
	ApplicationHandle = this;

	//Get the instance of this application
	m_hInstance = GetModuleHandle(NULL);

	//Give the Application a name
	m_applicationName = L"DX Practice Puru";

	//Setup the windows class with defaults
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	//Register window class
	RegisterClassEx(&wc);

	//Determine the resolution of clients desktop screen
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	if (GraphicsGlobals::FULL_SCREEN)
	{
		//if full screen set the size to max size of users desktop
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		//Change the display settings to full screen
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		//Set position of window to top left corner
		posY = posX = 0;
	}
	else
	{
		//If windows set it to 800x600
		screenWidth = 800;
		screenHeight = 600;

		//Place the window in the middle of the screen
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	//Create the window with screen settings and get its handle
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hInstance, NULL);

	//Bring window up to screen and set as focus
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	//Hide mouse 
	ShowCursor(false);
}

void PuruGameSystem::ShutdownWindows()
{
	//Show mouse cursor
	ShowCursor(true);

	//Fix display settings if leaving full screen
	if (GraphicsGlobals::FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	//Remove the window
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	//Remove the application instance
	UnregisterClass(m_applicationName, m_hInstance);
	m_hInstance = NULL;

	//Release the pointer to this class
	ApplicationHandle = NULL;

	return;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
		break;

	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
		break;
		
		//All other messages are passed onto Systems message handler
	default:
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		break;
	}
}
