#pragma once
#include "mesh_vm.g.h"
#include "bindable_base.h"

namespace winrt::transformations::implementation
{
	struct mesh_vm : mesh_vmT<mesh_vm, transformations::implementation::bindable_base>
	{
		mesh_vm() = default;

		Windows::Foundation::Numerics::float3 scale();
		void scale(Windows::Foundation::Numerics::float3 value);

	private:
		Windows::Foundation::Numerics::float3 m_scale;
	};
}
namespace winrt::transformations::factory_implementation
{
	struct mesh_vm : mesh_vmT<mesh_vm, implementation::mesh_vm>
	{
	};
}