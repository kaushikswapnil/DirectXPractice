#pragma once

//This class will handle texture for the font,
//the font data, and the function used to 
//build the vertex buffers with the font data
//The vertex buffers that hold the font data 
//for individual sentences will be in Text Class
//not this class

#include<d3d11.h>
#include<D3DX10math.h>
#include<fstream>

#include"TextureClass.h"

class FontClass
{
private:
	struct FontType
	{
		float left, right, top, bottom;
		int width, height;
	};
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};

public:
	FontClass();
	FontClass(const FontClass&);
	~FontClass();

	bool Initialize(ID3D11Device*, char*, WCHAR*);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();
	//BuildVertexArray will handle building and returning a
	//vertex array of triangles that will render the character
	//sequence which was given as input to this function
	//the function will be called by the new TextClass to
	//build vertex arrays of all sentences it needs to render
	void BuildVertexArray(void*, char*, float, float);

private:
	bool LoadFontData(char*);
	void ReleaseFontData();
	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();

private:
	FontType* m_Font;
	TextureClass* m_Texture;
};

