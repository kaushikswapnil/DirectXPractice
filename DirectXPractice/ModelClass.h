#pragma once

//The ModelClass is responsible for encapsulating the geometry for 3D models.

#include<d3d11.h>
#include<d3dx10math.h>
#include <vector>

class TextureClass;

class ModelClass
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture; //Used for texel coords
		D3DXVECTOR3 normal;
		//Old variables
		//D3DXVECTOR4 color; //Used to print color directly to the model. Not used for textures
	};


	//#TODO Find better names for ModelType
	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	bool Initialize(ID3D11Device* device, const char* modelFileName,const WCHAR* textureFileName);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();

	ID3D11ShaderResourceView* GetTexture();

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device* device, const WCHAR* fileName);
	void ReleaseTexture();

	bool LoadModel(const char* filename);

private:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	unsigned int m_VertexCount, m_IndexCount;

	TextureClass* m_Texture;
	std::vector<ModelType> m_ModelData; //#TODO find better name for this
};

