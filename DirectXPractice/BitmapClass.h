#pragma once

//The bitmap class will be used to represent an individual 2D image that needs 
//to be rendered to the screen. Every image needs a 2D class. This is similar to a
//model class but for 2D objects

#include<D3D11.h>
#include<D3DX10math.h>

#include"TextureClass.h"

class BitmapClass
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};
public:
	BitmapClass();
	BitmapClass(const BitmapClass&);
	~BitmapClass();

	bool Initialize(ID3D11Device* device, const int screenWidth, const int screenHeight, const WCHAR* tectureFilename, const int bitmapWidth, const int bitmapHeight);
	void Shutdown();
	bool Render(ID3D11DeviceContext *deviceContext, const int positionX, const int positionY);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	bool UpdateBuffers(ID3D11DeviceContext*, int, int);
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device* device, const WCHAR* filename);
	void ReleaseTexture();

private:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
	TextureClass *m_Texture;

	int m_screenWidth, m_screenHeight;
	int m_bitmapWidth, m_bitmapHeight;
	int m_previousPosX, m_previousPosY;
};

