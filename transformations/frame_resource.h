#pragma once

class frame_resource
{
public:
	frame_resource(winrt::com_ptr<ID3D12Device5> device, winrt::com_ptr<ID3D12PipelineState> pso);
	~frame_resource();

	uint64_t fence_value = 0;
	winrt::com_ptr<ID3D12CommandAllocator> cmd_allocator;
	winrt::com_ptr<ID3D12GraphicsCommandList4> cmd_list;
	winrt::com_ptr<ID3D12PipelineState> pso;
};

