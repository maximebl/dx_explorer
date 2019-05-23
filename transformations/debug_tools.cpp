#include "pch.h"
#include "debug_tools.h"

using namespace winrt;

winrt::com_ptr<ID3D12Debug1> debug_tools::m_debug_controller = nullptr;
winrt::com_ptr<ID3D12DebugCommandList2> debug_tools::m_debug_cmd_list = nullptr;
winrt::com_ptr<IDXGIDebug1> debug_tools::m_dxgi_debug = nullptr;

debug_tools::debug_tools()
{
}


debug_tools::~debug_tools()
{
}

void debug_tools::enable_debug_layer(bool is_gpu_validation_enabled)
{
	check_hresult(D3D12GetDebugInterface(winrt::guid_of<ID3D12Debug1>(), m_debug_controller.put_void()));
	check_hresult(DXGIGetDebugInterface1(0, guid_of<IDXGIDebug1>(), m_dxgi_debug.put_void()));
	m_debug_controller->EnableDebugLayer();
	m_debug_controller->SetEnableGPUBasedValidation(is_gpu_validation_enabled);
}

void debug_tools::track_leaks_for_thread()
{
	m_dxgi_debug->EnableLeakTrackingForThread();
}

bool debug_tools::assert_resource_state(ID3D12Resource* resource, D3D12_RESOURCE_STATES state)
{
	return m_debug_cmd_list->AssertResourceState(resource, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, state);
}
