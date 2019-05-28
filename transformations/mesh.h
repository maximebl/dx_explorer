#pragma once
using namespace winrt;

class mesh
{
public:
	mesh();
	~mesh();

	struct allocation
	{
		void* CPU;
		D3D12_GPU_VIRTUAL_ADDRESS GPU;
	};

	allocation vertex_allocation;
	allocation index_allocation;

	std::string name = "";

	winrt::com_ptr<ID3D12Resource> vertex_uploader = nullptr;
	winrt::com_ptr<ID3D12Resource> vertex_default = nullptr;
	UINT vertex_byte_stride = 0;
	UINT vertex_byte_size = 0;

	winrt::com_ptr<ID3D12Resource> index_uploader = nullptr;
	winrt::com_ptr<ID3D12Resource> index_default = nullptr;
	UINT index_byte_size = 0;
	DXGI_FORMAT index_format = DXGI_FORMAT::DXGI_FORMAT_R16_UINT;

	D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view()
	{
		D3D12_VERTEX_BUFFER_VIEW vbv;
		vbv.BufferLocation = vertex_default->GetGPUVirtualAddress();
		vbv.StrideInBytes = vertex_byte_stride;
		vbv.SizeInBytes = vertex_byte_size;
		return vbv;
	}

	D3D12_INDEX_BUFFER_VIEW index_buffer_view()
	{
		D3D12_INDEX_BUFFER_VIEW ibv;
		ibv.BufferLocation = index_default->GetGPUVirtualAddress();
		ibv.SizeInBytes = index_byte_size;
		ibv.Format = index_format;
	}

	void upload_to_gpu(ID3D12Device5* device, ID3D12GraphicsCommandList4* cmd_list, void* vertex_data, size_t vertex_byte_size);
};

