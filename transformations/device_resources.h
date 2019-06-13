#pragma once
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include "cmd_queue.h"

class device_resources
{
public:
	device_resources();
	~device_resources();

	void create_device(winrt::com_ptr<IDXGIAdapter4> adapter);
	void create_xaml_swapchain(std::shared_ptr<cmd_queue>& cmd_queue, winrt::Windows::UI::Xaml::Controls::SwapChainPanel swapchain_panel, winrt::com_ptr<IDXGIFactory7> dxgi_factory, UINT width, UINT height);

	void create_dsv_heap();
	void create_dsv(UINT64 width, UINT height);

	void create_rtv_heap();
	void create_rtv();

	void create_sampler_heap();
	void create_sampler();

	void transition_resource(ID3D12GraphicsCommandList4* cmd_list, ID3D12Resource* resource, D3D12_RESOURCE_STATES state_before, D3D12_RESOURCE_STATES state_after);
	void resize_rtv(UINT width, UINT height);
	uint32_t present();

	void create_default_buffer(ID3D12Device* device, 
		ID3D12GraphicsCommandList* cmd_list, 
		CD3DX12_RESOURCE_DESC* resource_desc, 
		const void* data, 
		ID3D12Resource** upload_buffer, 
		ID3D12Resource** default_buffer,
		hstring buffer_name);

	winrt::com_ptr<ID3DBlob> compile_shader_from_file(winrt::hstring filename, std::string entry_point, std::string target);
	ID3D12Resource* get_render_target(uint32_t index);
	D3D12_CPU_DESCRIPTOR_HANDLE get_current_rtv(uint32_t index);

	winrt::com_ptr<ID3D12Device> device;
	winrt::com_ptr<ID3D12DescriptorHeap> sampler_heap;
	winrt::com_ptr<ID3D12DescriptorHeap> dsv_heap;
	winrt::com_ptr<ID3D12DescriptorHeap> rtv_heap;
	constexpr static uint8_t buffer_count = 3;

private:
	winrt::com_ptr<ID3D12Resource> m_dsv;
	winrt::com_ptr<ID3D12Resource> m_cbv_uploaded_resource;
	winrt::com_ptr<IDXGISwapChain1> m_swapchain;
	std::array<winrt::com_ptr<ID3D12Resource>, buffer_count> m_swapchain_buffers;
	UINT m_rtv_increment_size;
	DXGI_FORMAT m_back_buffer_format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
};

