#pragma once
class LightClass
{
public:
	LightClass() : m_Direction(0.0f, 0.0f, 0.0f), m_DiffuseColor(0.0f, 0.0f, 0.0f, 0.0f), m_AmbientColor(0.0f, 0.0f, 0.0f, 0.0f) { }

	const D3DXVECTOR4& GetDiffuseColor() const { return m_DiffuseColor; }
	void SetDiffuseColor(const float red, const float green, const float blue, const float alpha) { m_DiffuseColor = D3DXVECTOR4(red, green, blue, alpha); }
	const D3DXVECTOR3& GetDirection() const { return m_Direction; }
	void SetDirection(const float x, const float y, const float z) { m_Direction = D3DXVECTOR3(x, y, z); }
    const D3DXVECTOR4& GetAmbientColor() const { return m_AmbientColor; }
    void SetAmbientColor(const float red, const float green, const float blue, const float alpha) { m_AmbientColor = D3DXVECTOR4(red, green, blue, alpha); }

private:
	D3DXVECTOR4 m_DiffuseColor;
	D3DXVECTOR3 m_Direction;
    D3DXVECTOR4 m_AmbientColor;
};

