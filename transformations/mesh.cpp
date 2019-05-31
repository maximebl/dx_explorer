#include "pch.h"
#include "mesh.h"

mesh::mesh()
{
}

mesh::~mesh()
{
}

void mesh::upload_to_gpu(ID3D12Device* device, ID3D12GraphicsCommandList4* cmd_list, void* vertex_data, UINT stride, UINT element_count)
{
	vertex_byte_stride = stride;
	vertex_count = element_count;
	vertex_byte_size = vertex_byte_stride * element_count;

	// cpu to upload heap
	check_hresult(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(vertex_byte_size),
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		guid_of<ID3D12Resource>(),
		vertex_uploader.put_void()));

	//TODO: should we keep a reference of the system data or just upload?
	vertex_uploader->Map(0, nullptr, &vertex_allocation.CPU);
	memcpy(vertex_allocation.CPU, vertex_data, vertex_byte_size);

	check_hresult(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(vertex_byte_size),
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		guid_of<ID3D12Resource>(),
		vertex_default.put_void()));

	D3D12_SUBRESOURCE_DATA subresource_data;
	subresource_data.pData = vertex_allocation.CPU;
	subresource_data.RowPitch = vertex_byte_size;
	subresource_data.SlicePitch = vertex_byte_size;

	cmd_list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		vertex_default.get(),
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST));

	// upload heap to gpu default heap
	UpdateSubresources(cmd_list, vertex_default.get(), vertex_uploader.get(), 0, 0, 1, &subresource_data);

	cmd_list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		vertex_default.get(),
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ));
}
