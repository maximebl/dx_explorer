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
	check_hresult(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_1, guid_of<ID3D12Device>(), device.put_void()));
	m_rtv_increment_size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_cbv_srv_uav_increment_size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void device_resources::create_xaml_swapchain(std::shared_ptr<cmd_queue>& cmd_queue, winrt::Windows::UI::Xaml::Controls::SwapChainPanel swapchain_panel, com_ptr<IDXGIFactory7> dxgi_factory, UINT width, UINT height)
{
	DXGI_SWAP_CHAIN_DESC1 swapchain_desc;
	swapchain_desc.AlphaMode = DXGI_ALPHA_MODE::DXGI_ALPHA_MODE_IGNORE;
	swapchain_desc.BufferCount = buffer_count;
	swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapchain_desc.Flags = DXGI_SWAP_CHAIN_FLAG::DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	swapchain_desc.Format = m_back_buffer_format;
	swapchain_desc.Width = width;
	swapchain_desc.Height = height;
	swapchain_desc.SampleDesc.Count = 1;
	swapchain_desc.SampleDesc.Quality = 0;
	swapchain_desc.Scaling = DXGI_SCALING::DXGI_SCALING_STRETCH;
	swapchain_desc.Stereo = false;
	swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;

	check_hresult(dxgi_factory->CreateSwapChainForComposition(cmd_queue->get_cmd_queue().get(), &swapchain_desc, nullptr, m_swapchain.put()));
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
	dsv_resource_desc.Format = m_dsv_res_format;
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
	optimized_clear_value.Format = m_dsv_res_format;

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
	dsv_desc.Format = m_dsv_res_format;
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

void device_resources::compile_default_shaders()
{
	hstring file_name = L"default_shader.hlsl";
	auto shaders_folder = winrt::Windows::ApplicationModel::Package::Current().InstalledLocation().Path() + L"\\shaders\\";
	default_shaders[L"vs"] = compile_shader_from_file(shaders_folder + file_name, "VS", "vs_5_1");
	default_shaders[L"ps"] = compile_shader_from_file(shaders_folder + file_name, "PS", "ps_5_1");
	default_shaders[L"vs_noproj"] = compile_shader_from_file(shaders_folder + file_name, "VS_NoProj", "vs_5_1");
	default_shaders[L"vs_outline"] = compile_shader_from_file(shaders_folder + file_name, "VS_Outline", "vs_5_1");
	default_shaders[L"ps_outline"] = compile_shader_from_file(shaders_folder + file_name, "PS_Outline", "ps_5_1");
}

void device_resources::create_default_rootsig()
{
	//D3D12_DESCRIPTOR_RANGE cb_manips_ranges[1];
	//D3D12_DESCRIPTOR_RANGE sampler_ranges[1];
	//D3D12_ROOT_PARAMETER params[3];

	//D3D12_DESCRIPTOR_RANGE descriptor_range_cb;
	//descriptor_range_cb.BaseShaderRegister = 1;
	//descriptor_range_cb.NumDescriptors = 1;
	//descriptor_range_cb.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//indicates this range should immediately follow the preceding range.
	//descriptor_range_cb.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	//descriptor_range_cb.RegisterSpace = 0;
	//cb_manips_ranges[0] = descriptor_range_cb;

	//D3D12_ROOT_DESCRIPTOR_TABLE cb_manips_table;
	//cb_manips_table.NumDescriptorRanges = 1;
	//cb_manips_table.pDescriptorRanges = cb_manips_ranges;

	//D3D12_ROOT_PARAMETER cb_manips_param;
	//cb_manips_param.ParameterType = D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	//cb_manips_param.ShaderVisibility = D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_VERTEX;
	//cb_manips_param.DescriptorTable = cb_manips_table;
	//params[0] = cb_manips_param;

	//// Samplers cannot be mixed with other resource types in a descriptor table (root parameter [0])
	//D3D12_DESCRIPTOR_RANGE descriptor_range_sampler;
	//descriptor_range_sampler.BaseShaderRegister = 0;
	//descriptor_range_sampler.NumDescriptors = 1;
	//descriptor_range_sampler.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//indicates this range should immediately follow the preceding range.
	//descriptor_range_sampler.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
	//descriptor_range_sampler.RegisterSpace = 0;
	//sampler_ranges[0] = descriptor_range_sampler;

	//D3D12_ROOT_DESCRIPTOR_TABLE sampler_table;
	//sampler_table.NumDescriptorRanges = 1;
	//sampler_table.pDescriptorRanges = sampler_ranges;

	//D3D12_ROOT_PARAMETER root_param_sampler;
	//root_param_sampler.ParameterType = D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	//root_param_sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_VERTEX;
	//root_param_sampler.DescriptorTable = sampler_table;
	//params[1] = root_param_sampler;

	//D3D12_ROOT_DESCRIPTOR view_proj_cb_root_descriptor;
	//view_proj_cb_root_descriptor.RegisterSpace = 0;
	//view_proj_cb_root_descriptor.ShaderRegister = 0;

	//D3D12_ROOT_PARAMETER cb_view_proj_param;
	//cb_view_proj_param.ParameterType = D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_CBV;
	//cb_view_proj_param.ShaderVisibility = D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_VERTEX;
	//cb_view_proj_param.Descriptor = view_proj_cb_root_descriptor;
	//params[2] = cb_view_proj_param;

	//D3D12_ROOT_SIGNATURE_DESC rootsig_desc;
	//rootsig_desc.Flags = D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	//rootsig_desc.NumParameters = _countof(params);
	//rootsig_desc.NumStaticSamplers = 0;
	//rootsig_desc.pParameters = params;

	//ID3DBlob* rootsig_blob = nullptr;
	//ID3DBlob* error_blob = nullptr;

	//hresult hr = D3D12SerializeRootSignature(&rootsig_desc, D3D_ROOT_SIGNATURE_VERSION::D3D_ROOT_SIGNATURE_VERSION_1, &rootsig_blob, &error_blob);
	//if (error_blob != nullptr)
	//{
	//	auto msg_ptr = static_cast<const char*>(error_blob->GetBufferPointer());
	//	hstring error_msg = winrt::to_hstring(msg_ptr);
	//}
	//check_hresult(hr);

	//check_hresult(device->CreateRootSignature(
	//	0,
	//	rootsig_blob->GetBufferPointer(),
	//	rootsig_blob->GetBufferSize(),
	//	guid_of<ID3D12RootSignature>(),
	//	default_root_signature.put_void()));
	CD3DX12_ROOT_SIGNATURE_DESC rootsig_desc(
		0,
		nullptr,
		0,
		nullptr,
		D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	com_ptr<ID3DBlob> serialized_rootsig = nullptr;
	com_ptr<ID3DBlob> error_blob = nullptr;
	D3D12SerializeRootSignature(&rootsig_desc, D3D_ROOT_SIGNATURE_VERSION::D3D_ROOT_SIGNATURE_VERSION_1, serialized_rootsig.put(), error_blob.put());

	if (error_blob != nullptr)
	{
		auto error_msg_ptr = static_cast<const char*>(error_blob->GetBufferPointer());
		winrt::hstring message = winrt::to_hstring(error_msg_ptr);
	}

	check_hresult(device->CreateRootSignature(
		0,
		serialized_rootsig->GetBufferPointer(),
		serialized_rootsig->GetBufferSize(),
		guid_of<ID3D12RootSignature>(),
		default_root_signature.put_void()));
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

void device_resources::create_line_pso()
{
	D3D12_INPUT_ELEMENT_DESC input_elements[2] = {};
	D3D12_INPUT_ELEMENT_DESC positions_input_element;
	positions_input_element.SemanticName = "POSITION";
	positions_input_element.SemanticIndex = 0;
	positions_input_element.Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT;
	positions_input_element.InputSlot = 0;
	positions_input_element.AlignedByteOffset = 0;
	positions_input_element.InputSlotClass = D3D12_INPUT_CLASSIFICATION::D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	positions_input_element.InstanceDataStepRate = 0;
	input_elements[0] = positions_input_element;

	D3D12_INPUT_ELEMENT_DESC colors_input_element;
	colors_input_element.SemanticName = "COLOR";
	colors_input_element.SemanticIndex = 0;
	colors_input_element.Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT;
	colors_input_element.InputSlot = 0;
	colors_input_element.AlignedByteOffset = 12;
	colors_input_element.InputSlotClass = D3D12_INPUT_CLASSIFICATION::D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	colors_input_element.InstanceDataStepRate = 0;
	input_elements[1] = colors_input_element;

	D3D12_INPUT_LAYOUT_DESC input_layout;
	input_layout.NumElements = 2;
	input_layout.pInputElementDescs = input_elements;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc = {};
	pso_desc.InputLayout = input_layout;
	pso_desc.pRootSignature = default_root_signature.get();
	pso_desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	auto rast_state = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	rast_state.CullMode = D3D12_CULL_MODE::D3D12_CULL_MODE_NONE;
	pso_desc.RasterizerState = rast_state;
	pso_desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	pso_desc.DSVFormat = m_dsv_res_format;
	pso_desc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE::D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
	pso_desc.NumRenderTargets = 1;
	pso_desc.RTVFormats[0] = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
	pso_desc.NodeMask = 0;
	pso_desc.Flags = D3D12_PIPELINE_STATE_FLAGS::D3D12_PIPELINE_STATE_FLAG_NONE;
	pso_desc.SampleMask = UINT_MAX;
	pso_desc.SampleDesc.Count = 1;
	pso_desc.SampleDesc.Quality = 0;
	pso_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;

	//shaders
	D3D12_SHADER_BYTECODE vs_bytecode;
	vs_bytecode.BytecodeLength = default_shaders[L"vs_noproj"]->GetBufferSize();
	vs_bytecode.pShaderBytecode = default_shaders[L"vs_noproj"]->GetBufferPointer();

	D3D12_SHADER_BYTECODE ps_bytecode;
	ps_bytecode.BytecodeLength = default_shaders[L"ps"]->GetBufferSize();
	ps_bytecode.pShaderBytecode = default_shaders[L"ps"]->GetBufferPointer();

	pso_desc.VS = vs_bytecode;
	pso_desc.PS = ps_bytecode;

	com_ptr<ID3D12PipelineState> line_pso = nullptr;
	check_hresult(device->CreateGraphicsPipelineState(&pso_desc, guid_of<ID3D12PipelineState>(), line_pso.put_void()));
	default_psos[L"line"] = line_pso;
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

void device_resources::resize_rtv(UINT width, UINT height)
{
	for (int i = 0; i < m_swapchain_buffers.size(); ++i)
	{
		m_swapchain_buffers.at(i) = nullptr;
	}
	//DXGI ERROR: IDXGISwapChain::ResizeBuffers: 
	//Swapchain cannot be resized unless all outstanding buffer references have been released. 
	check_hresult(m_swapchain->ResizeBuffers(
		buffer_count,
		width,
		height,
		m_back_buffer_format,
		DXGI_SWAP_CHAIN_FLAG::DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
	));

	D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle = rtv_heap->GetCPUDescriptorHandleForHeapStart();

	for (int i = 0; i < m_swapchain_buffers.size(); ++i)
	{
		check_hresult(m_swapchain->GetBuffer(i, guid_of<ID3D12Resource>(), m_swapchain_buffers[i].put_void()));
		device->CreateRenderTargetView(m_swapchain_buffers[i].get(), nullptr, rtv_handle);
		rtv_handle.ptr = rtv_handle.ptr + m_rtv_increment_size;
	}

	m_dsv = nullptr;
	create_dsv(width, height);
}

uint32_t device_resources::present()
{
	hresult hr = m_swapchain->Present(0, 0);
	if (hr)
	{
		check_hresult(device->GetDeviceRemovedReason());
		throw_hresult(hr);
	}
	return m_swapchain.as<IDXGISwapChain4>()->GetCurrentBackBufferIndex();
}

void device_resources::create_default_buffer(
	ID3D12GraphicsCommandList* cmd_list,
	CD3DX12_RESOURCE_DESC* resource_desc,
	const void* data,
	ID3D12Resource** upload_buffer,
	ID3D12Resource** default_buffer,
	hstring buffer_name)
{
	check_hresult(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
		resource_desc,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		guid_of<ID3D12Resource>(),
		(void**)upload_buffer));

	check_hresult(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
		resource_desc,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		guid_of<ID3D12Resource>(),
		(void**)default_buffer));

	check_hresult((*default_buffer)->SetName(buffer_name.c_str()));

	D3D12_SUBRESOURCE_DATA subresource_data;
	subresource_data.pData = data;
	subresource_data.RowPitch = resource_desc->Width;
	subresource_data.SlicePitch = subresource_data.RowPitch;

	// upload heap to gpu default heap
	UpdateSubresources(cmd_list, *default_buffer, *upload_buffer, 0, 0, 1, &subresource_data);

	cmd_list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		*default_buffer,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ));
}

void device_resources::create_upload_buffer(ID3D12GraphicsCommandList* cmd_list, CD3DX12_RESOURCE_DESC* resource_desc, void** data, size_t byte_size, ID3D12Resource** upload_buffer)
{
	check_hresult(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
		resource_desc,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		guid_of<ID3D12Resource>(),
		(void**)upload_buffer));

	check_hresult((*upload_buffer)->SetName(L"line_vertex_uploader"));

	D3D12_RANGE range;
	range.Begin = 0;
	range.End = byte_size;

	check_hresult((*upload_buffer)->Map(0, nullptr, reinterpret_cast<void**>(data)));
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
