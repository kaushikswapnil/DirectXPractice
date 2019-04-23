#include "GraphicsClass.h"
#include "Assertion.h"
#include "ModelClass.h"
#include "ColorShaderClass.h"
#include "LogSystem.h"
#include <Assertion.h>
#include "LightShaderClass.h"
#include "LightClass.h"

#define RETURN_FALSE_IF_FALSE(condition) \
do \
{	\
	if (!(condition)) \
	{ \
		return false; \
	} \
} while (false)

#define LOG_IF_FALSE(condition, message) \
do \
{	\
	if (!(condition)) \
	{ \
		LOG(message); \
	} \
} while (false)

#define LOG_RETURN_IF_FALSE(condition, message) \
do \
{	\
	if (!(condition)) \
	{ \
		std::string messageStr(message); \
		LOG(messageStr); \
		return false; \
	} \
} while (false)




GraphicsClass::GraphicsClass() : m_D3D(nullptr), m_Camera(nullptr), m_Model(nullptr), m_LightShader(nullptr), m_Light(nullptr) //m_TextureShader(nullptr), m_Text(nullptr), m_Model(nullptr), m_ColorShader(nullptr)
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
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);

	m_Model = new ModelClass;
	LOG_RETURN_IF_FALSE(m_Model != nullptr, "Could not create model class");

	//Initialize the model class 
	result = m_Model->Initialize(m_D3D->GetDevice(), L"flowers.dds");
	HARDASSERT(result, "Could not initialize model class");

	//Create the light shader
	m_LightShader = new LightShaderClass();
	HARDASSERT(m_LightShader != nullptr, "Unable to create Light Shader");

	result = m_LightShader->Initialize(m_D3D->GetDevice(), hwnd);
	HARDASSERT(result, "Unable to initialize light shader");

	m_Light = new LightClass();
	HARDASSERT(m_Light != nullptr, "Unable to create light");
	m_Light->SetDiffuseColor(1.0f, 0.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);

	//Create + Initialize texture shader
	/*m_TextureShader = new TextureShaderClass;
	HARDASSERT(m_TextureShader != nullptr, "Could not create texture shader");

	result = m_TextureShader->Initialize(m_D3D->GetDevice(), hwnd);
	HARDASSERT(result, "Could not initialize texture shader");
*/
	//For color shading
	////Create the color shader
	//m_ColorShader = new ColorShaderClass;
	//LOG_RETURN_IF_FALSE(m_ColorShader != nullptr, "Could not create color shader object");

	//result = m_ColorShader->Initialize(m_D3D->GetDevice(), hwnd);
	//LOG_RETURN_IF_FALSE(result, "Could not initialize color shader");
	
	return true;
}

void GraphicsClass::Shutdown()
{
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = nullptr;
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

	if (m_Light)
	{
		delete m_Light;
		m_Light = nullptr;
	}

	if (m_LightShader)
	{
		delete m_LightShader;
		m_LightShader = nullptr;
	}
}

bool GraphicsClass::Frame()
{
	bool result;

	static float angleOfRotation = 0.0f;

	angleOfRotation += (float)D3DX_PI * 0.01f;
	if (angleOfRotation > 360.0f)
	{
		angleOfRotation = 0.0f;
	}

	//Render the graphics scene
	result = Render(angleOfRotation);
	if (!result)
	{
		return false;
	}

	return true;
}

bool GraphicsClass::Render(const float angleOfRotation)
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	bool result;


	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	//Get the world, view, an dprojection matrices from the camera and d3d objects
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	//Rotate the world matrix by the rotation value so that the shape wil spin
	D3DXMatrixRotationY(&worldMatrix, angleOfRotation);

	//Put the model vertex and index bugffers on the graphics pipeline to prepare them for drawing
	m_Model->Render(m_D3D->GetDeviceContext());

	//Render the model using the color shader
	result = m_LightShader->Render(m_D3D->GetDeviceContext(), 
								m_Model->GetIndexCount(), 
								worldMatrix, 
								viewMatrix, 
								projectionMatrix, 
								m_Model->GetTexture(),
								m_Light->GetDirection(), m_Light->GetDiffuseColor());
	if (!result)
	{
		return false;
	}

	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
}


//Code for rendering 2d in Render()
	//2D TEXT RENDERING
	//// Get the view, projection, and world matrices from the camera and D3D objects.
	//m_Camera->GetViewMatrix(viewMatrix);
	//m_D3D->GetWorldMatrix(worldMatrix);
	//m_D3D->GetProjectionMatrix(projectionMatrix);
	//m_D3D->GetOrthoMatrix(orthoMatrix);

	//// Turn off the Z buffer to begin all 2D rendering.
	//m_D3D->TurnZBufferOff();

	//// Turn on the alpha blending before rendering the text.
	//m_D3D->TurnOnAlphaBlending();

	//// Render the text strings.
	//result = m_Text->Render(m_D3D->GetDeviceContext(), worldMatrix, orthoMatrix);
	//if (!result)
	//{
	//	return false;
	//}

	//// Turn off alpha blending after rendering the text.
	//m_D3D->TurnOffAlphaBlending();

	//// Turn the Z buffer back on now that all 2D rendering has completed.
	//m_D3D->TurnZBufferOn();