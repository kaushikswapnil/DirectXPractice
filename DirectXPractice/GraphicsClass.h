#pragma once

#include<windows.h>

#include"D3DClass.h"
#include"CameraClass.h"
#include"BitmapClass.h"
#include"TextureShaderClass.h"
#include"TextClass.h"

namespace GraphicsGlobals
{
	const bool FULL_SCREEN = false;
	const bool VSYNC_ENABLED = true;
	const float SCREEN_DEPTH = 1000.0f;
	const float SCREEN_NEAR = 0.1f;
}

class ModelClass;
class ColorShaderClass;

class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();

private:
	bool Render();

private:
	D3DClass* m_D3D;
	CameraClass* m_Camera;
	BitmapClass* m_Bitmap;
	TextureShaderClass* m_TextureShader;
	TextClass* m_Text;
	ModelClass* m_Model;
	ColorShaderClass* m_ColorShader; //used for color shading
};

