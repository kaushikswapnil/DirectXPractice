#include "ModelClass.h"
#include "TextureClass.h"
#include "Assertion.h"
#include <fstream>

ModelClass::ModelClass() : m_vertexBuffer(nullptr), m_indexBuffer(nullptr), m_IndexCount(0), m_VertexCount(0), m_Texture(nullptr), m_ModelData()
{
}

ModelClass::ModelClass(const ModelClass &)
{
}


ModelClass::~ModelClass()
{
}

bool ModelClass::Initialize(ID3D11Device *device, const char* modelFileName, const WCHAR* textureFileName)
{
	bool result;

	result = LoadModel(modelFileName);
	HARDASSERT(result, "Unable to load the model data file");

	//Initialize the vertex and index buffer that hold geometry
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	result = LoadTexture(device, textureFileName);
	if (!result)
	{
		return false;
	}

	return true;
}

void ModelClass::Shutdown()
{
	ReleaseTexture();

	ShutdownBuffers();

	return;
}

//render us called from the GraphicsClass render function
//This calls the RenderBuffers to put the vertex and index
//buffers on the graphics pipeline so that color shader
//can render them.
void ModelClass::Render(ID3D11DeviceContext * deviceContext)
{
	//Put the buffers on graphics pipeline
	RenderBuffers(deviceContext);

	return;
}

int ModelClass::GetIndexCount()
{
	return m_IndexCount;
}

ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_Texture->GetTexture();
}

//This is where we handle creating the vertex and index buffers
//Usually a model file is create and buffers are created from there
bool ModelClass::InitializeBuffers(ID3D11Device *device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	//create the vertex array
	vertices = new VertexType[m_VertexCount];
	if (!vertices)
	{
		return false;
	}

	//Create the index array
	indices = new unsigned long[m_IndexCount];
	if (!indices)
	{
		return false;
	}

	// Load the vertex array with data.
	for (unsigned int iter = 0; iter < m_IndexCount; ++iter)
	{
		vertices[iter].position = D3DXVECTOR3(m_ModelData[iter].x, m_ModelData[iter].y, m_ModelData[iter].z);
		vertices[iter].texture = D3DXVECTOR2(m_ModelData[iter].tu, m_ModelData[iter].tv);
		vertices[iter].normal = D3DXVECTOR3(m_ModelData[iter].nx, m_ModelData[iter].ny, m_ModelData[iter].nz);

		indices[iter] = iter;
	}


	//With the vertex and index array filled out, we can now create the vertex buffer 
	//and index buffer. First we fill out description of the buffer.
	//In desc, the byte width and bind flags are what you need to ensure are filled out correctly.
	//After the desc is filled out you also have to fill a sub resource pointer to point to the
	//vertex or index array created. After that the buffer can be created via create buffer

	//Set up the desc of the static vertex buffer
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_VertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	//Give the sub resource structure a pointer to the vertex data
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	//Now create the vertex buffer
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//Set up the description of the static index buffer
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_IndexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	//Give sub resource structure a pointer to the index data
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	//Create the index buffer
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//Release the arrays now that buffers are created
	delete[] vertices;
	vertices = nullptr;

	delete[] indices;
	indices = nullptr;

	return true;
}

void ModelClass::ShutdownBuffers()
{
	//Releases the buffers
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = nullptr;
	}
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
	}

	return;
}

//This is called from the render function.
//its purpose is to set the vertex and index buffer as active on the
//input assembler in GPU. Once GPU has an active vertex buffer it can then use the shader
//to render that buffer. This function also defines how those buffers should be drawn
//(triangles, lines, fans, so on)
void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

bool ModelClass::LoadTexture(ID3D11Device* device, const WCHAR* fileName)
{
	bool result;

	//Create the texture object
	m_Texture = new TextureClass;
	HARDASSERT(m_Texture != nullptr, "Could not create texture");

	result = m_Texture->Initialize(device, fileName);
	HARDASSERT(result, "Could not initialize texture class");

	return true;
}

void ModelClass::ReleaseTexture()
{
	HARDASSERT(m_Texture != nullptr, "Trying to release a non-loaded texture");
	m_Texture->Shutdown();
	delete m_Texture;
	m_Texture = nullptr;
}

bool ModelClass::LoadModel(const char* filename)
{
	char input;
	std::ifstream modelDataFile(filename);
	HARDASSERT(!(modelDataFile.fail()), "Could not open model file");

	//Try to fetch the vertex buffer count
	do 
	{
		modelDataFile.get(input);
	} while (input != ':');

	modelDataFile >> m_VertexCount;

	m_IndexCount = m_VertexCount;

	do 
	{
		modelDataFile.get(input);
	} while (input != ':');

	modelDataFile.get(input);
	modelDataFile.get(input);

	//Pre allocate data for vertex
	m_ModelData.reserve(m_VertexCount);

	for (unsigned int iter = 0; iter < m_VertexCount; ++iter)
	{
		ModelType vertexData;
		modelDataFile >> vertexData.x >> vertexData.y >> vertexData.z;
		modelDataFile >> vertexData.tu >> vertexData.tv;
		modelDataFile >> vertexData.nx >> vertexData.ny >> vertexData.nz;
		m_ModelData.push_back(vertexData);
	}

	//modelDataFile.close();

	return true;
}
