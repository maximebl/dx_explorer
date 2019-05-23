#include "pch.h"
#include "system_info.h"

using namespace winrt;

system_info::system_info()
{
}


system_info::~system_info()
{
}

winrt::com_ptr<IDXGIAdapter4> system_info::get_high_performance_adapter()
{
	check_hresult(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, guid_of<IDXGIFactory7>(), dxgi_factory.put_void()));

	winrt::com_ptr<IDXGIAdapter4> adapter = nullptr;
	check_hresult(dxgi_factory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE::DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, guid_of<IDXGIAdapter4>(), adapter.put_void()));

	return adapter;
}
