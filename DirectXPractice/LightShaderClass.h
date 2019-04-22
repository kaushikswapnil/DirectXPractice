#pragma once
#include "IShaderClass.h"

class LightShaderClass :
    public IShaderClass
{

public:
    LightShaderClass();
    ~LightShaderClass();

protected:
    struct LightBufferType
    {
        D3DXVECTOR4 diffuseColor;
        D3DXVECTOR3 lightDirection;
        float padding; //Added extra padding so structure is a multiple of 16  fpr CreateBuffer function requirements
    };
};

