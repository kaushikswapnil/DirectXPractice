#include "LightShaderClass.h"
#include "Assertion.h"
#include <fstream>

bool LightShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
    bool result = InitializeShader(device, hwnd, L"light.vs", L"light.ps");
    HARDASSERT(result, "Could not initialize shader");
	return result;
}

void LightShaderClass::Shutdown()
{
    ShutdownShader();
}

bool LightShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, const D3DXVECTOR3& lightDirection, const D3DXVECTOR4& ambientColor, const D3DXVECTOR4& diffuseColor, const D3DXVECTOR3 cameraPosition, const D3DXVECTOR4 specularColor, const float specularPower)
{
    bool result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, lightDirection, ambientColor, diffuseColor, cameraPosition, specularColor, specularPower);
    HARDASSERT(result, "Unable to set shader parameters");

    RenderShader(deviceContext, indexCount);


    //#TODO Maybe this should be a void stle function now
    return true;
}

bool LightShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFileName, WCHAR* psFileName)
{
    HRESULT result;
    ID3D10Blob *errorMessage = nullptr, *vertexShaderBuffer = nullptr, *pixelShaderBuffer = nullptr;
    D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
    unsigned int numElements;
    D3D11_SAMPLER_DESC samplerDesc;
    D3D11_BUFFER_DESC matrixBufferDesc;
    D3D11_BUFFER_DESC lightBufferDesc;
    D3D11_BUFFER_DESC cameraBufferDesc;

    //Compile the vertex shader code
    result = D3DX11CompileFromFile(vsFileName, NULL, NULL, "LightVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &vertexShaderBuffer, &errorMessage, NULL);

    if (FAILED(result))
    {
        // If the shader failed to compile it should have writen something to the error message.
        if (errorMessage)
        {
            OutputShaderErrorMessage(errorMessage, hwnd, vsFileName);
        }
        // If there was nothing in the error message then it simply could not find the shader file itself.
        else
        {
            MessageBox(hwnd, vsFileName, L"Missing Shader File", MB_OK);
        }

        return false;
    }

    //Compile the pixel shader code
    result = D3DX11CompileFromFile(psFileName, NULL, NULL, "LightPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &pixelShaderBuffer, &errorMessage, NULL);

    if (FAILED(result))
    {
        // If the shader failed to compile it should have writen something to the error message.
        if (errorMessage)
        {
            OutputShaderErrorMessage(errorMessage, hwnd, psFileName);
        }
        // If there was nothing in the error message then it simply could not find the shader file itself.
        else
        {
            MessageBox(hwnd, psFileName, L"Missing Shader File", MB_OK);
        }

        return false;
    }

    //Create shaders from buffer
    result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_VertexShader);
    HARDASSERT(!FAILED(result), "Could not create vertex shader");

    result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_PixelShader);
    HARDASSERT(!FAILED(result), "Could not create vertex shader");

    //Create the vertex input layout description
    //This setup must match the VertexType described in Model, and in the shader
    polygonLayout[0].SemanticName = "POSITION";
    polygonLayout[0].SemanticIndex = 0;
    polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[0].InputSlot = 0;
    polygonLayout[0].AlignedByteOffset = 0;
    polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[0].InstanceDataStepRate = 0;

    polygonLayout[1].SemanticName = "TEXCOORD";
    polygonLayout[1].SemanticIndex = 0;
    polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    polygonLayout[1].InputSlot = 0;
    polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT; //Append to previous element
    polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[1].InstanceDataStepRate = 0;

    polygonLayout[2].SemanticName = "NORMAL";
    polygonLayout[2].SemanticIndex = 0;
    polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[2].InputSlot = 0;
    polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[2].InstanceDataStepRate = 0;

    // Get a count of the elements in the layout.
    numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

    result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_Layout);
    HARDASSERT(!FAILED(result), "Could not create shader input layout"); 

    //Reader shader buffers as we dont need them now
    vertexShaderBuffer->Release();
    pixelShaderBuffer->Release();
    vertexShaderBuffer = pixelShaderBuffer = nullptr;

    //Create a texture sampler state
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.BorderColor[0] = 0;
    samplerDesc.BorderColor[1] = 0;
    samplerDesc.BorderColor[2] = 0;
    samplerDesc.BorderColor[3] = 0;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    //Create a new sampler state
    result = device->CreateSamplerState(&samplerDesc, &m_SampleState);
    HARDASSERT(!FAILED(result), "Could not create sampler state");

    //Setup the description of the dynamic matrix constant buffer  described in the vertex shader
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;

    //create the constant buffer pointer
    result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_MatrixBuffer);
    HARDASSERT(!FAILED(result), "Unable to create matrix buffer");

    // Setup the description of the light dynamic constant buffer that is in the pixel shader.
    // Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
    lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    lightBufferDesc.ByteWidth = sizeof(LightBufferType);
    lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    lightBufferDesc.MiscFlags = 0;
    lightBufferDesc.StructureByteStride = 0;

    // Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
    result = device->CreateBuffer(&lightBufferDesc, NULL, &m_LightBuffer);
    HARDASSERT(!FAILED(result), "Unable to create light buffer");

    //Setup desc for camera buffer
    cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
    cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cameraBufferDesc.MiscFlags = 0;
    cameraBufferDesc.StructureByteStride = 0;

    //create camera buffer
    result = device->CreateBuffer(&cameraBufferDesc, NULL, &m_CameraBuffer);
    HARDASSERT(!FAILED(result), "Unable to create camera buffer");

    return true;
}

void LightShaderClass::ShutdownShader()
{
    HARDASSERT(m_LightBuffer != nullptr, "Trying to release and shutdown a non-existant buffer");
    m_LightBuffer->Release();
    m_LightBuffer = nullptr;

    HARDASSERT(m_MatrixBuffer != nullptr, "Trying to release and shutdown a non-existant buffer");
    m_MatrixBuffer->Release();
    m_MatrixBuffer = nullptr;

    HARDASSERT(m_CameraBuffer != nullptr, "Trying to release and shutdown a non-existant buffer");
    m_CameraBuffer->Release();
    m_CameraBuffer = nullptr;

    HARDASSERT(m_SampleState != nullptr, "Trying to release and shutdown a non-existant buffer");
    m_SampleState->Release();
    m_SampleState = nullptr;

    HARDASSERT(m_Layout != nullptr, "Trying to release and shutdown a non-existant buffer");
    m_Layout->Release();
    m_Layout = nullptr;

    HARDASSERT(m_PixelShader != nullptr, "Trying to release and shutdown a non-existant buffer");
    m_PixelShader->Release();
    m_PixelShader = nullptr;

    HARDASSERT(m_VertexShader != nullptr, "Trying to release and shutdown a non-existant buffer");
    m_VertexShader->Release();
    m_VertexShader = nullptr;
}

void LightShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFileName)
{
    char* compileErrors;
    unsigned long bufferSize, i;
    std::ofstream fout;


    // Get a pointer to the error message text buffer.
    compileErrors = (char*)(errorMessage->GetBufferPointer());

    // Get the length of the message.
    bufferSize = errorMessage->GetBufferSize();

    // Open a file to write the error message to.
    fout.open("shader-error.txt");

    // Write out the error message.
    for (i = 0; i < bufferSize; i++)
    {
        fout << compileErrors[i];
    }

    // Close the file.
    fout.close();

    // Release the error message.
    errorMessage->Release();
    errorMessage = 0;

    // Pop a message up on the screen to notify the user to check the text file for compile errors.
    MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFileName, MB_OK);

    return;
}

bool LightShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, const D3DXVECTOR3& lightDirection, const D3DXVECTOR4& ambientColor, const D3DXVECTOR4& diffuseColor, const D3DXVECTOR3 cameraPosition, const D3DXVECTOR4 specularColor, const float specularPower)
{
    HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    unsigned int bufferNumber;
    MatrixBufferType* matrixBufferDataPtr = nullptr;
    LightBufferType* lightBufferDataPtr = nullptr;
    CameraBufferType* cameraBufferDataPtr = nullptr;

    // Transpose the matrices to prepare them for the shader.
    D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
    D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
    D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	//Lock the constant buffer so it can be written to
	result = deviceContext->Map(m_MatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	HARDASSERT(!FAILED(result), "Unable to map the matrix buffer");

	//Get a ptr to the data in the constant buffer
	matrixBufferDataPtr = static_cast<MatrixBufferType*>(mappedResource.pData);

	//copy the matrices into the constant buffer
	matrixBufferDataPtr->world = worldMatrix;
	matrixBufferDataPtr->view = viewMatrix;
	matrixBufferDataPtr->projection = projectionMatrix;

	//Unlock the constant buffer
	deviceContext->Unmap(m_MatrixBuffer, 0);

	//Set the position of the constant buffer in the vertex shader
	bufferNumber = 0;

	//Now set the constant buffer in the vertex shader with the updated values
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_MatrixBuffer);

    //Map the camera position buffer
    result = deviceContext->Map(m_CameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    HARDASSERT(!FAILED(result), "Could not map camera buffer");

    cameraBufferDataPtr = static_cast<CameraBufferType*>(mappedResource.pData);

    cameraBufferDataPtr->cameraPosition = cameraPosition;
    cameraBufferDataPtr->padding = 0.0f;

    deviceContext->Unmap(m_CameraBuffer, 0);

    bufferNumber = 1; //This is because in the vertex shader, matrix buffer is at 0 position and camera buffer is at position 1

    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_CameraBuffer);

	//Set the shader texture resource in the pixel shader
	deviceContext->PSSetShaderResources(0, 1, &texture);

	//Lock the light constant buffer
	result = deviceContext->Map(m_LightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	HARDASSERT(!FAILED(result), "Unable the map the light buffer");

	//Get a ptr to the data in the constant buffer
	lightBufferDataPtr = static_cast<LightBufferType*>(mappedResource.pData);

	//Copy the lighting variables into the constant buffer
    lightBufferDataPtr->ambientColor = ambientColor;
	lightBufferDataPtr->diffuseColor = diffuseColor;
	lightBufferDataPtr->lightDirection = lightDirection;
	lightBufferDataPtr->specularColor = specularColor;
    lightBufferDataPtr->specularPower = specularPower;

	//Unlock the constant buffer
	deviceContext->Unmap(m_LightBuffer, 0);

	//Set the position of the light constant buffer in the pixel shader
	bufferNumber = 0;

	//Set the light constant buffer in the pixel shader with updated values
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_LightBuffer);

	return true;
}

void LightShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	//Set the vertex input layout
	deviceContext->IASetInputLayout(m_Layout);

	//Set the vertex and pixel shaders that will be used to render this triangle
	deviceContext->VSSetShader(m_VertexShader, NULL, 0);
	deviceContext->PSSetShader(m_PixelShader, NULL, 0);

	//Set the sampler state in the pixel shader
	deviceContext->PSSetSamplers(0, 1, &m_SampleState);

	//Render the triangle
	deviceContext->DrawIndexed(indexCount, 0, 0);
}

