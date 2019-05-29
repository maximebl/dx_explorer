#include "pch.h"
#include "device_resources.h"
#include <windows.ui.xaml.media.dxinterop.h>
#include "ui_helpers.h"

using namespace winrt;

device_resources::device_resources()
{
}

device_resources::~device_resources()
{
}

void device_resources::create_device(winrt::com_ptr<IDXGIAdapter4> adapter)
{
	check_hresult(D3D12CreateDevice(adapter.get(), D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_1, guid_of<ID3D12Device5>(), device.put_void()));
	m_rtv_increment_size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
}

void device_resources::create_cmd_queue()
{
	command_queue = std::make_shared<cmd_queue>(device);
}

void device_resources::create_xaml_swapchain(winrt::Windows::UI::Xaml::Controls::SwapChainPanel swapchain_panel, com_ptr<IDXGIFactory7> dxgi_factory, UINT width, UINT height)
{
	DXGI_SWAP_CHAIN_DESC1 swapchain_desc;
	swapchain_desc.AlphaMode = DXGI_ALPHA_MODE::DXGI_ALPHA_MODE_IGNORE;
	swapchain_desc.BufferCount = buffer_count;
	swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapchain_desc.Flags = 0;
	swapchain_desc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchain_desc.Width = width;
	swapchain_desc.Height = height;
	swapchain_desc.SampleDesc.Count = 1;
	swapchain_desc.SampleDesc.Quality = 0;
	swapchain_desc.Scaling = DXGI_SCALING::DXGI_SCALING_STRETCH;
	swapchain_desc.Stereo = false;
	swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;

	check_hresult(dxgi_factory->CreateSwapChainForComposition(command_queue->get_cmd_queue().get(), &swapchain_desc, nullptr, m_swapchain.put()));
	check_hresult(swapchain_panel.as<ISwapChainPanelNative>()->SetSwapChain(m_swapchain.get()));
}

void device_resources::create_dsv_heap()
{
	D3D12_DESCRIPTOR_HEAP_DESC dsv_heap_desc = {};
	dsv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsv_heap_desc.NodeMask = 0;
	dsv_heap_desc.NumDescriptors = 1;
	dsv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

	check_hresult(
		device->CreateDescriptorHeap(&dsv_heap_desc, guid_of<ID3D12DescriptorHeap>(), dsv_heap.put_void())
	);
}

void device_resources::create_dsv(UINT64 width, UINT height)
{
	D3D12_RESOURCE_DESC dsv_resource_desc;
	dsv_resource_desc.Alignment = 0;
	dsv_resource_desc.DepthOrArraySize = 1;
	dsv_resource_desc.Dimension = D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	dsv_resource_desc.Flags = D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	dsv_resource_desc.Format = DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
	dsv_resource_desc.Height = height;
	dsv_resource_desc.Width = width;
	dsv_resource_desc.Layout = D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_UNKNOWN;
	dsv_resource_desc.MipLevels = 1;
	dsv_resource_desc.SampleDesc.Count = 1;
	dsv_resource_desc.SampleDesc.Quality = 0;

	D3D12_HEAP_PROPERTIES heap_props;
	heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY::D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heap_props.CreationNodeMask = 0;
	heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL::D3D12_MEMORY_POOL_UNKNOWN;
	heap_props.Type = D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_DEFAULT;
	heap_props.VisibleNodeMask = 0;

	D3D12_CLEAR_VALUE optimized_clear_value = {};
	optimized_clear_value.DepthStencil.Depth = 1.0f;
	optimized_clear_value.DepthStencil.Stencil = 0;
	optimized_clear_value.Format = DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;

	check_hresult(device->CreateCommittedResource(
		&heap_props,
		D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
		&dsv_resource_desc,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&optimized_clear_value,
		guid_of<ID3D12Resource>(),
		m_dsv.put_void()));

	D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc = {};
	dsv_desc.Flags = D3D12_DSV_FLAGS::D3D12_DSV_FLAG_NONE;
	dsv_desc.Format = DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
	dsv_desc.Texture2D.MipSlice = 0;
	dsv_desc.ViewDimension = D3D12_DSV_DIMENSION::D3D12_DSV_DIMENSION_TEXTURE2D;

	device->CreateDepthStencilView(m_dsv.get(), &dsv_desc, dsv_heap->GetCPUDescriptorHandleForHeapStart());
}

void device_resources::create_rtv_heap()
{
	D3D12_DESCRIPTOR_HEAP_DESC rtv_heap_desc = {};
	rtv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtv_heap_desc.NodeMask = 0;
	rtv_heap_desc.NumDescriptors = buffer_count;
	rtv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

	check_hresult(
		device->CreateDescriptorHeap(&rtv_heap_desc, guid_of<ID3D12DescriptorHeap>(), rtv_heap.put_void())
	);
}

void device_resources::create_rtv()
{
	UINT rtv_descriptor_size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_CPU_DESCRIPTOR_HANDLE rtv_heap_handle;
	rtv_heap_handle.ptr = rtv_heap->GetCPUDescriptorHandleForHeapStart().ptr;

	for (int i = 0; i < buffer_count; ++i)
	{
		check_hresult(m_swapchain->GetBuffer(i, guid_of<ID3D12Resource>(), m_swapchain_buffers[i].put_void()));
		m_swapchain_buffers[i]->SetName((L"render target " + std::to_wstring(i)).c_str());
		device->CreateRenderTargetView(m_swapchain_buffers[i].get(), nullptr, rtv_heap_handle);
		rtv_heap_handle.ptr += rtv_descriptor_size;
	}
}

ID3D12Resource* device_resources::get_render_target(uint32_t index)
{
	return m_swapchain_buffers[index].get();
}

D3D12_CPU_DESCRIPTOR_HANDLE device_resources::get_current_rtv(uint32_t index)
{
	D3D12_CPU_DESCRIPTOR_HANDLE current_rtv_handle;
	current_rtv_handle.ptr = rtv_heap->GetCPUDescriptorHandleForHeapStart().ptr + (index * m_rtv_increment_size);
	return current_rtv_handle;
}

void device_resources::create_sampler_heap()
{
	D3D12_DESCRIPTOR_HEAP_DESC sampler_heap_desc = {};
	sampler_heap_desc.NodeMask = 0;
	sampler_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	sampler_heap_desc.NumDescriptors = 1;
	sampler_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
	check_hresult(device->CreateDescriptorHeap(&sampler_heap_desc, guid_of<ID3D12DescriptorHeap>(), sampler_heap.put_void()));
}

void device_resources::create_sampler()
{
	D3D12_SAMPLER_DESC sampler_desc = {};

	memcpy(sampler_desc.BorderColor, DirectX::Colors::Black, sizeof(FLOAT) * 4);
	sampler_desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	sampler_desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	sampler_desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	sampler_desc.Filter = D3D12_ENCODE_ANISOTROPIC_FILTER(D3D12_FILTER_REDUCTION_TYPE::D3D12_FILTER_REDUCTION_TYPE_STANDARD);
	sampler_desc.MaxAnisotropy = D3D12_MAX_MAXANISOTROPY;
	sampler_desc.MaxLOD = D3D12_FLOAT32_MAX;
	sampler_desc.MinLOD = 0.f;
	sampler_desc.MipLODBias = 0.0f;
	device->CreateSampler(&sampler_desc, sampler_heap->GetCPUDescriptorHandleForHeapStart());
}

void device_resources::transition_resource(ID3D12GraphicsCommandList4* cmd_list, ID3D12Resource* resource, D3D12_RESOURCE_STATES state_before, D3D12_RESOURCE_STATES state_after)
{
	D3D12_RESOURCE_TRANSITION_BARRIER transition;
	transition.pResource = resource;
	transition.Subresource = 0;
	transition.StateBefore = state_before;
	transition.StateAfter = state_after;

	D3D12_RESOURCE_BARRIER barrier;
	barrier.Transition = transition;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAGS::D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE::D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	cmd_list->ResourceBarrier(1, &barrier);
}

uint32_t device_resources::present()
{
	check_hresult(m_swapchain->Present(0, 0));
	return m_swapchain.as<IDXGISwapChain4>()->GetCurrentBackBufferIndex();
}

com_ptr<ID3DBlob> device_resources::compile_shader_from_file(hstring filename, std::string entry_point, std::string target)
{
	UINT compile_flags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
	compile_flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	com_ptr<ID3DBlob> errors;
	com_ptr<ID3DBlob> shader_byte_code;

	hresult hr = D3DCompileFromFile(
		filename.c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entry_point.c_str(),
		target.c_str(),
		compile_flags,
		0,
		shader_byte_code.put(),
		errors.put()
	);

	if (errors != nullptr)
	{
		auto msg_ptr = static_cast<const char*>(errors->GetBufferPointer());
		hstring error_msg = winrt::to_hstring(msg_ptr);
		ui_helpers::show_error_dialog(error_msg, L"Failed to compile shader : " + filename);
	}

	check_hresult(hr);
	return shader_byte_code;
}
