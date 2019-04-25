#pragma once
#include "IShaderClass.h"
#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>

class LightShaderClass :
    public IShaderClass
{
public:
    LightShaderClass() : IShaderClass(), m_VertexShader(nullptr), m_PixelShader(nullptr), m_Layout(nullptr), m_SampleState(nullptr), m_MatrixBuffer(nullptr), m_LightBuffer(nullptr) {}
    LightShaderClass(const LightShaderClass& other) = delete;
    ~LightShaderClass() = default;

    bool Initialize(ID3D11Device* device, HWND hwnd);
    void Shutdown();
    bool Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, const D3DXVECTOR3& lightDirection, const D3DXVECTOR4& ambientColor, const D3DXVECTOR4& diffuseColor);

private:
    bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFileName, WCHAR* psFileName);
    void ShutdownShader();
    void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFileName);
    bool SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, const D3DXVECTOR3& lightDirection, const D3DXVECTOR4& ambientColor, const D3DXVECTOR4& diffuseColor);
    void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);

protected:
	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};

    struct LightBufferType
    {
        D3DXVECTOR4 ambientColor;
        D3DXVECTOR4 diffuseColor;
        D3DXVECTOR3 lightDirection;
        float padding; //Added extra padding so structure is a multiple of 16  fpr CreateBuffer function requirements
    };

    ID3D11VertexShader* m_VertexShader;
    ID3D11PixelShader*  m_PixelShader;
    ID3D11InputLayout* m_Layout;
    ID3D11SamplerState* m_SampleState;
    ID3D11Buffer* m_MatrixBuffer;
    ID3D11Buffer* m_LightBuffer;
};

