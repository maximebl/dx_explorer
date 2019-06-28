#pragma once
#include <dxgidebug.h>
#include <dxgi1_6.h>

class debug_tools
{
public:
	debug_tools();
	~debug_tools();

	static void enable_debug_layer(bool is_gpu_validation_enabled);
	static bool assert_resource_state(ID3D12Resource* resource, D3D12_RESOURCE_STATES state);
	static void track_leaks_for_thread();
	static void draw_debug_line(ID3D12GraphicsCommandList4* cmd_list, ID3D12Device5* device );

private:
	static winrt::com_ptr<ID3D12Debug1> m_debug_controller;
	static winrt::com_ptr<ID3D12DebugCommandList2> m_debug_cmd_list;
	static winrt::com_ptr<IDXGIDebug1> m_dxgi_debug;

};

