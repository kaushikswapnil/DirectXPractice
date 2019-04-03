#include "ColorShaderClass.h"



ColorShaderClass::ColorShaderClass() : m_vertexShader(nullptr), m_pixelShader(nullptr), m_layout(nullptr), m_matrixBuffer(nullptr)
{
}

ColorShaderClass::ColorShaderClass(const ColorShaderClass &)
{
}


ColorShaderClass::~ColorShaderClass()
{
}

//Calls the shader initialization function. We pass names of HLSL shader files
bool ColorShaderClass::Initialize(ID3D11Device *device, HWND hwnd)
{
	bool result;
	
	//Initialize vector and pixel shaders
	result = InitializeShader(device, hwnd, L"color.vs", L"color.ps");
	if (!result)
	{
		return false;
	}

	return true;
}

void ColorShaderClass::Shutdown()
{
	//Shutdown the shaders
	ShutdownShader();

	return;
}

bool ColorShaderClass::Render(ID3D11DeviceContext *deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix)
{
	bool result;

	//Set the shader parameters that it will use for rendering
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}

	//Now render the prepared buffers with shader
	RenderShader(deviceContext, indexCount);

	return true;
}

//This function loads the shader files and makes it usable to DX and GPU.
bool ColorShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFileName, WCHAR* psFileName)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;

	//Initialize the pointers this function will use to null
	errorMessage = nullptr;
	vertexShaderBuffer = nullptr;
	pixelShaderBuffer = nullptr;

	//Compile the shader programs into buffers. We give it the shader file name, shader name, and version
	//And the buffer to compile the shader to.

	//Compile the vertex shader code
	result = D3DX11CompileFromFile(vsFileName, NULL, NULL, "ColorVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL,
		&vertexShaderBuffer, &errorMessage, NULL);
	if (FAILED(result))
	{
		//If the shader failed to compile it should have written something to the error message
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFileName);
		}
		//Otherwise it simply couldn't find the shader file
		else
		{
			MessageBox(hwnd, vsFileName, L"Missing Shader File", MB_OK);
		}

		return false;
	}
	
	//Compile the pixel shader code
	result = D3DX11CompileFromFile(psFileName, NULL, NULL, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL,
		&pixelShaderBuffer, &errorMessage, NULL);
	if(FAILED(result))
	{
		//If the shader failed to compile it should have written something to the error message
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFileName);
		}
		//Otherwise it simply couldn't find the shader file
		else
		{
			MessageBox(hwnd, psFileName, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	//Once the vertex and pixel shader code has been compiled into buffers, we can use those buffers
	//to create the shader objects themselves

	//Create the vertex shader from buffer
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	//Create the pixel shader from buffer
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	//Now we create the layout of the vertex data that will be processed by the shader. As this shader uses a position and color vector
	//we need to create both in the layout  specifying the size of both. The semantic name is the first thing to fill out in the layout.
	//This allows the shader to determine the usage of this element of the layout. As we have two different elements we use position for the
	//first one and color for the second. 
	//The next important attribute is Format. For position we use DXGI_FORMAT_R32G32B32A32_FLOAT and for color DXGI_FORMAT)R31G31B31A32_FLOAT
	//Then we set AllignedByteOffset which indicates how the data is spaced in buffer. For this layout we are telling it the first 12 bytes are position
	//And next 16 are color. AllignedByteOffset shows where each element begins. D3D11_APPEND_ALIGNED_ELEMENT can be used instead of configuring it ourselves,
	//and it will figure out spacing .

	//Setup the layout of the data that goes into the shader
	//This needs to match the VertexType structure in ModelClass and shader
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	//Now we can get the size of it and create the input layout
	//using the d3d device. Also release vertex and pixel buffers as they are not
	//needed after creation of layout

	//Get count of elements in layout
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	//Create the vertex input layout
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
	{
		return false;
	}

	//Release the vertex shader buffer and pixel shader buffer
	vertexShaderBuffer->Release();
	vertexShaderBuffer = nullptr;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = nullptr;

	//The final thing that needs to be set up to utilize the shader is the constant buffer
	//The buffer usage is set to dynamic as we will be updating it each frame
	//The bind flags indicate that this buffer will be a constant buffer
	//The CPU access flags need to match up with the usage so it is set to D3D11_CPU_ACCESS_WRITE
	//Once we fill out the desc we can create the buffer interface and use it to access
	//the internal variables in the shader using the function SetShaderParameters

	//Setup the desc of the dynamic matrix constant buffer thats in the vertex shader
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	//Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void ColorShaderClass::ShutdownShader()
{
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = nullptr;
	}

	if (m_layout)
	{
		m_layout->Release();
		m_layout = nullptr;
	}

	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = nullptr;
	}

	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = nullptr;
	}

	return;
}

void ColorShaderClass::OutputShaderErrorMessage(ID3D10Blob *errorMessage, HWND hwnd, WCHAR *shaderFileName)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	std::ofstream fout;

	//Get a pointer to the error message text buffer
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	//Get length of message
	bufferSize = errorMessage->GetBufferSize();

	//Open a file to write the error message to 
	fout.open("shader-error.txt");

	//Write  out the error message;
	for (i = 0; i < bufferSize; ++i)
	{
		fout << compileErrors[i];
	}

	//close the file
	fout.close();

	//Release the error Message
	errorMessage->Release();
	errorMessage = nullptr;

	//Pop a message box
	MessageBox(hwnd, L"Error compiling shader. Check shader-error.txt for message.", shaderFileName, MB_OK);

	return;
}

//This functions sets the global variables in shader. The matrices used are created in Graphics Class
bool ColorShaderClass::SetShaderParameters(ID3D11DeviceContext *deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	//transpose the matrices
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	//Lock the matrix buffer
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	//Get a pointer to the data in constant buffer
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	//Copy the matrices into constant buffer;
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	//unlock buffer
	deviceContext->Unmap(m_matrixBuffer, 0);

	//now set the updated buffer in the HLSL vertex shader

	//Set the position of constant buffer in vertex shader
	bufferNumber = 0;

	//Finally set the constant buffer in the vertex shader with the updated values
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);


	return true;
}


//This is the second function called in the render function. SetShaderParameters is called before this to ensure that
//the shader parameters are setup correctly,
//First we set our input layout to active in the input assembler. This lets the GPU
//know the format of data in vertex buffer. Next we set vertex shader and pixel shader
//we will be using to render the vertex buffer. Once the shaders are set, we render the 
//triangle calling the DrawIndexed function, which will render the green triangle
void ColorShaderClass::RenderShader(ID3D11DeviceContext *deviceContext, int indexCount)
{
	//Set the vertex input layout
	deviceContext->IASetInputLayout(m_layout);

	//Set the vertex and pixel shaders that will be used to render the triangle
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	//Render the triangle
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}
