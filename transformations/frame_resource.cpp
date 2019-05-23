#include "pch.h"
#include "frame_resource.h"

using namespace winrt;

frame_resource::frame_resource(winrt::com_ptr<ID3D12Device5> device, winrt::com_ptr<ID3D12PipelineState> pso) : pso(pso)
{
	check_hresult(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT,
		guid_of<ID3D12CommandAllocator>(),
		cmd_allocator.put_void()));

	check_hresult(device->CreateCommandList(0,
		D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT,
		cmd_allocator.get(),
		pso.get(),
		guid_of<ID3D12GraphicsCommandList4>(),
		cmd_list.put_void()));

	cmd_list->Close();
}


frame_resource::~frame_resource()
{
}
