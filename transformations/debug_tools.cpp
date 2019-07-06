#include "pch.h"
#include "debug_tools.h"

using namespace winrt;

debug_tools::debug_tools()
{
}

debug_tools::~debug_tools()
{
}

void debug_tools::enable_debug_layer(debug_mode mode)
{
	check_hresult(D3D12GetDebugInterface(winrt::guid_of<ID3D12Debug1>(), m_debug_controller.put_void()));
	check_hresult(DXGIGetDebugInterface1(0, guid_of<IDXGIDebug1>(), m_dxgi_debug.put_void()));
	m_debug_controller->EnableDebugLayer();

	if (mode == debug_mode::enable_gpu_validation)
	{
		m_debug_controller->SetEnableGPUBasedValidation(true);
	}
}

void debug_tools::track_leaks_for_thread()
{
	m_dxgi_debug->EnableLeakTrackingForThread();
}

bool debug_tools::assert_resource_state(ID3D12Resource* resource, D3D12_RESOURCE_STATES state)
{
	return m_debug_cmd_list->AssertResourceState(resource, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, state);
}
