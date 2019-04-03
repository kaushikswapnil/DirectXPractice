#include "GraphicsClass.h"



GraphicsClass::GraphicsClass() : m_D3D(nullptr), m_Camera(nullptr), m_Bitmap(nullptr), m_TextureShader(nullptr), m_Text(nullptr)
{
}

GraphicsClass::GraphicsClass(const GraphicsClass &)
{
}


GraphicsClass::~GraphicsClass()
{
}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;
	D3DXMATRIX baseViewMatrix;

	//Create the d3d object
	m_D3D = new D3DClass();
	if (!m_D3D)
	{
		return false;
	}

	//Initialize the Direct3D Object
	result = m_D3D->Initialize(screenWidth, screenHeight, GraphicsGlobals::VSYNC_ENABLED, hwnd, GraphicsGlobals::FULL_SCREEN, GraphicsGlobals::SCREEN_DEPTH, GraphicsGlobals::SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	//Create new camera
	m_Camera = new CameraClass();
	if (!m_Camera)
	{
		return false;
	}

	//Initialize a base view matrix with the camera for 2D UI rendering
	m_Camera->SetPosition(0.0f, 0.0f, -1.0f);
	m_Camera->Render();
	m_Camera->GetViewMatrix(baseViewMatrix);

	// Create the text object.
	m_Text = new TextClass;
	if (!m_Text)
	{
		return false;
	}

	// Initialize the text object.
	result = m_Text->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, baseViewMatrix);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the text object.", L"Error", MB_OK);
		return false;
	}

	//Set initial position of camera
	//m_Camera->SetPosition(0.0f, 0.0f, -10.0f);

	//m_TextureShader = new TextureShaderClass();
	//if (!m_TextureShader)
	//{
	//	return false;
	//}

	//result = m_TextureShader->Initialize(m_D3D->GetDevice(), hwnd);
	//if (!result)
	//{
	//	MessageBox(hwnd, L"Could not initialize TextureShader Object", L"Error", MB_OK);
	//	return false;
	//}

	////Create the Bitmap object
	//m_Bitmap = new BitmapClass();
	//if (!m_Bitmap)
	//{
	//	return false;
	//}

	///*result = m_Bitmap->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, L"gamebg.dds", 256, 256);
	//if (!result)
	//{
	//	MessageBox(hwnd, L"Could not initialize Bitmap Object", L"Error", MB_OK);
	//	return false;
	//}*/
	//result = m_Bitmap->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, L"galaga_sheet.dds", 24, 24);
	//if (!result)
	//{
	//	MessageBox(hwnd, L"Could not initialize Bitmap Object", L"Error", MB_OK);
	//	return false;
	//}

	return true;
}

void GraphicsClass::Shutdown()
{
	if (m_Text)
	{
		m_Text->Shutdown();
		delete m_Text;
		m_Text = nullptr;
	}

	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;;
		m_TextureShader = nullptr;
	}
	
	if (m_Bitmap)
	{
		m_Bitmap->Shutdown();
		delete m_Bitmap;
		m_Bitmap = nullptr;
	}

	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = nullptr;
	}
	
	if (m_D3D)
	{
		delete m_D3D;
		m_D3D = nullptr;
	}
}

bool GraphicsClass::Frame()
{
	bool result;

	//Render the graphics scene
	result = Render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool GraphicsClass::Render()
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	bool result;


	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the view, projection, and world matrices from the camera and D3D objects.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);

	// Turn off the Z buffer to begin all 2D rendering.
	m_D3D->TurnZBufferOff();

	// Turn on the alpha blending before rendering the text.
	m_D3D->TurnOnAlphaBlending();

	// Render the text strings.
	result = m_Text->Render(m_D3D->GetDeviceContext(), worldMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

	////Put model vertex and index buffers on graphics pipeline to prepare for drawing
	//result = m_Bitmap->Render(m_D3D->GetDeviceContext(), 100, 100);
	//if (!result)
	//{
	//	return false;
	//}

	////Render the model using the color shader
	//result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Bitmap->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_Bitmap->GetTexture());
	//if (!result)
	//{
	//	return false;
	//}

	// Turn off alpha blending after rendering the text.
	m_D3D->TurnOffAlphaBlending();

	// Turn the Z buffer back on now that all 2D rendering has completed.
	m_D3D->TurnZBufferOn();

	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
}
