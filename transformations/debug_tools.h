#pragma once
#include <dxgidebug.h>
#include <dxgi1_6.h>
#include "../external_libs/d3dx12.h"

class debug_tools
{
public:
	debug_tools();
	~debug_tools();

	enum class debug_mode {
		enable_gpu_validation = 0
	};

	void enable_debug_layer(debug_mode mode);
	bool assert_resource_state(ID3D12Resource* resource, D3D12_RESOURCE_STATES state);
	void track_leaks_for_thread();

private:
	winrt::com_ptr<ID3D12Debug1> m_debug_controller;
	winrt::com_ptr<ID3D12DebugCommandList2> m_debug_cmd_list;
	winrt::com_ptr<IDXGIDebug1> m_dxgi_debug;
	winrt::com_ptr<ID3D12PipelineState> m_line_pso;
};

