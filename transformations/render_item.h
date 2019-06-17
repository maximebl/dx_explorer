#pragma once
#include "mesh.h"
class render_item
{
public:
	render_item();
	~render_item();

	struct allocation
	{
		void* CPU;
		D3D12_GPU_VIRTUAL_ADDRESS GPU;
	};

	allocation constant_buffer_allocation;
	mesh mesh_geometry;
	winrt::com_ptr<ID3D12Resource> model_cbv_uploader;
};

