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
	UINT vertex_count;
	D3D12_VERTEX_BUFFER_VIEW vbv;

	winrt::com_ptr<ID3D12Resource> index_uploader = nullptr;
	winrt::com_ptr<ID3D12Resource> index_default = nullptr;
	UINT index_count = 0;
	UINT index_byte_size = 0;
	DXGI_FORMAT index_format = DXGI_FORMAT::DXGI_FORMAT_R16_UINT;
	D3D12_INDEX_BUFFER_VIEW ibv;
};

