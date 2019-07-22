#pragma once
#include "mesh.h"
#include <DirectXCollision.h>

class render_item
{
public:
	render_item();
	~render_item();

	struct allocation
	{
		std::byte* CPU;
		D3D12_GPU_VIRTUAL_ADDRESS GPU;
	};

	DirectX::BoundingBox bounding_box;
	D3D12_GPU_DESCRIPTOR_HANDLE cbv_gpu_handle;
	D3D12_CPU_DESCRIPTOR_HANDLE cbv_cpu_handle;
	allocation model_allocation;
	allocation srt_allocation;
	mesh mesh_geometry;
	winrt::com_ptr<ID3D12Resource> model_cbv_uploader;
	winrt::com_ptr<ID3D12Resource> srt_cbv_uploader;
};

