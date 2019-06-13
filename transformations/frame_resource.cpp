#include "pch.h"
#include "frame_resource.h"

using namespace winrt;

frame_resource::frame_resource(winrt::com_ptr<ID3D12Device> device) 
{
	check_hresult(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT,
		guid_of<ID3D12CommandAllocator>(),
		cmd_allocator.put_void()));

	check_hresult(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT,
		guid_of<ID3D12CommandAllocator>(),
		ui_requests_cmd_allocator.put_void()));
}


frame_resource::~frame_resource()
{
}
