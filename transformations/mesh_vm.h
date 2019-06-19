#pragma once
#include "mesh_vm.g.h"
#include "bindable_base.h"
#include "float3.h"

namespace winrt::transformations::implementation
{
	struct mesh_vm : mesh_vmT<mesh_vm, transformations::implementation::bindable_base>
	{
		mesh_vm() = default;

		int32_t index = 0;
		transformations::float3 scale();
		void scale(transformations::float3 value);

	private:
		transformations::float3 m_scale = winrt::make<transformations::implementation::float3>(1.0f, 1.0f, 1.0f);
	};
}

namespace winrt::transformations::factory_implementation
{
	struct mesh_vm : mesh_vmT<mesh_vm, implementation::mesh_vm>
	{
	};
}