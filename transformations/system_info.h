#pragma once
#include <dxgi1_6.h>

class system_info
{
public:
	system_info();
	~system_info();

	winrt::com_ptr<IDXGIFactory7> dxgi_factory;
	winrt::com_ptr<IDXGIAdapter4> get_high_performance_adapter();
};

