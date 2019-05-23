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
	void create_cmd_queue();
	void create_xaml_swapchain(winrt::Windows::UI::Xaml::Controls::SwapChainPanel swapchain_panel, winrt::com_ptr<IDXGIFactory7> dxgi_factory);

	void create_dsv_heap();
	void create_dsv();

	void create_rtv_heap();
	void create_rtv();

	void create_sampler_heap();
	void create_sampler();

	void transition_resource(ID3D12GraphicsCommandList4* cmd_list, ID3D12Resource* resource, D3D12_RESOURCE_STATES state_before, D3D12_RESOURCE_STATES state_after);
	uint32_t present();

	winrt::com_ptr<ID3DBlob> compile_shader_from_file(winrt::hstring filename, std::string entry_point, std::string target);
	ID3D12Resource* get_render_target(uint32_t index);
	D3D12_CPU_DESCRIPTOR_HANDLE get_current_rtv(uint32_t index);

	std::shared_ptr<cmd_queue> command_queue;
	winrt::com_ptr<ID3D12Device5> device;
	winrt::com_ptr<ID3D12DescriptorHeap> sampler_heap;
	uint64_t output_width = 512;
	uint64_t output_height = 512;
	winrt::com_ptr<ID3D12DescriptorHeap> dsv_heap;
	winrt::com_ptr<ID3D12DescriptorHeap> rtv_heap;
	constexpr static uint8_t buffer_count = 3;

private:
	winrt::com_ptr<ID3D12Resource> m_dsv;
	winrt::com_ptr<ID3D12Resource> m_cbv_uploaded_resource;
	winrt::com_ptr<IDXGISwapChain1> m_swapchain;
	std::array<winrt::com_ptr<ID3D12Resource>, buffer_count> m_swapchain_buffers;
	UINT m_rtv_increment_size;
};

