#include "pch.h"
#include "float3.h"
#include "float3.g.cpp"

namespace winrt::transformations::implementation
{
	float3::float3(float x, float y, float z)
	{
		m_x = x;
		m_y = y;
		m_z = z;
	}

	float float3::x()
	{
		return m_x;
	}

	void float3::x(float value)
	{
		update_value(L"x", m_x, value);
	}

	float float3::y()
	{
		return m_y;
	}

	void float3::y(float value)
	{
		update_value(L"y", m_y, value);
	}

	float float3::z()
	{
		return m_z;
	}

	void float3::z(float value)
	{
		update_value(L"z", m_z, value);
	}
}
