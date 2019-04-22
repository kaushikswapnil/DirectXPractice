#pragma once
#include <D3DX10math.h>

class IShaderClass
{
public:
    IShaderClass() = default;
    virtual ~IShaderClass() = default;
    IShaderClass(const IShaderClass& other) = delete;


protected:
    struct MatrixBufferType
    {
        D3DXMATRIX world;
        D3DXMATRIX view;
        D3DXMATRIX projection;
    };
};