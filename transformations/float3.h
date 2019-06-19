#pragma once
#include "float3.g.h"
#include "bindable_base.h"

namespace winrt::transformations::implementation
{
    struct float3 : float3T<float3, transformations::implementation::bindable_base>
    {
        float3() = default;
		float3(float x, float y, float z);

		float x();
        void x(float value);
        float y();
        void y(float value);
        float z();
        void z(float value);

	private:
		float m_x = 0.0f;
		float m_y = 0.0f;
		float m_z = 0.0f;
    };
}

namespace winrt::transformations::factory_implementation
{
    struct float3 : float3T<float3, implementation::float3>
    {
    };
}