#pragma once

class IShaderClass
{
public:
    IShaderClass() = default;
    virtual ~IShaderClass() = default;
    IShaderClass(const IShaderClass& other) = delete;

};