#include "pch.h"
#include "mesh_vm.h"
#include "mesh_vm.g.cpp"

using namespace winrt;
using namespace winrt::Windows::Foundation::Numerics;

namespace winrt::transformations::implementation
{
	transformations::float3 mesh_vm::scale()
	{
		return m_scale;
	}

	void mesh_vm::scale(transformations::float3 value)
	{
		update_value(L"scale", m_scale, value);
	}
}