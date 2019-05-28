#include "pch.h"
#include "mvp_showcase_app.h"

using namespace winrt;
using namespace winrt::Windows::Foundation;

mvp_showcase_app::mvp_showcase_app()
{
	m_system_info = system_info();
	m_device_resources = device_resources();
}

void mvp_showcase_app::compile_shaders()
{
	com_ptr<ID3DBlob> new_vertex_shader = nullptr;
	hstring vs_shader_name = L"default_shader.hlsl";
	auto shaders_folder = winrt::Windows::ApplicationModel::Package::Current().InstalledLocation().Path() + L"\\shaders\\";
	m_shaders[vs_shader_name] = m_device_resources.compile_shader_from_file(shaders_folder + vs_shader_name, "VS", "vs_5_1");
}

mvp_showcase_app::~mvp_showcase_app()
{
}

bool mvp_showcase_app::initialize(winrt::Windows::UI::Xaml::Controls::SwapChainPanel swapchain_panel)
{
#if defined(_DEBUG)
	debug_tools::enable_debug_layer(true);
	debug_tools::track_leaks_for_thread();
#endif

	if (!DirectX::XMVerifyCPUSupport())
	{
		return false;
	}

	com_ptr<IDXGIAdapter4> adapter = m_system_info.get_high_performance_adapter();

	if (adapter)
	{
		m_device_resources.create_device(adapter);
		m_device_resources.create_cmd_queue();
		m_device_resources.create_dsv_heap();
		m_device_resources.create_dsv();
		m_device_resources.create_xaml_swapchain(swapchain_panel, m_system_info.dxgi_factory);
		m_device_resources.create_rtv_heap();
		m_device_resources.create_rtv();
		m_device_resources.create_sampler_heap();
		m_device_resources.create_sampler();
		create_srv_cbv_uav_heap(1);
		build_frame_resources();
		create_root_signature();
	}
}

void mvp_showcase_app::load_content()
{
	compile_shaders();
	create_mvp_cbv();
	create_pso();
	create_cube();
}

DWORD __stdcall mvp_showcase_app::record_cmd_lists(void* instance)
{
	mvp_showcase_app* app = reinterpret_cast<mvp_showcase_app*>(instance);

	while (true)
	{
		app->update();
		app->render();
	}
	return 0;
}

void mvp_showcase_app::render()
{
	auto cmd_queue = m_device_resources.command_queue;
	auto cmd_allocator = m_current_frame_resource->cmd_allocator;
	auto cmd_list = m_current_frame_resource->cmd_list;
	auto pso = m_current_frame_resource->pso;

	check_hresult(cmd_allocator->Reset());
	check_hresult(cmd_list->Reset(cmd_allocator.get(), pso.get()));

	D3D12_VIEWPORT viewport;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<float>(m_device_resources.output_width);
	viewport.Height = static_cast<float>(m_device_resources.output_height);
	viewport.MaxDepth = 1.0f;
	viewport.MinDepth = 0.f;
	cmd_list->RSSetViewports(1, &viewport);

	// --set the scissor rect

	m_device_resources.transition_resource(cmd_list.get(), m_device_resources.get_render_target(m_current_backbuffer_index),
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET);

	D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle = m_device_resources.get_current_rtv(m_current_backbuffer_index);
	D3D12_CPU_DESCRIPTOR_HANDLE dsv_handle = m_device_resources.dsv_heap->GetCPUDescriptorHandleForHeapStart();

	cmd_list->ClearRenderTargetView(rtv_handle, DirectX::Colors::BlueViolet, 0, nullptr);
	//cmd_list->ClearDepthStencilView(m_device_resources.dsv_heap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	cmd_list->OMSetRenderTargets(1, &rtv_handle, true, &dsv_handle);

	cmd_list->SetGraphicsRootSignature(m_root_signature.get());

	std::array<ID3D12DescriptorHeap*, 2> descriptor_heaps = { m_srv_cbv_uav_heap.get(), m_device_resources.sampler_heap.get() };
	cmd_list->SetDescriptorHeaps(static_cast<UINT>(descriptor_heaps.size()), descriptor_heaps.data());

	//set the descriptor tables

	// draw the cube render_item

	m_device_resources.transition_resource(cmd_list.get(), m_device_resources.get_render_target(m_current_backbuffer_index),
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT);

	cmd_queue->execute_cmd_list(cmd_list);

	m_current_backbuffer_index = m_device_resources.present();

	m_current_frame_resource->fence_value = cmd_queue->signal();
}

void mvp_showcase_app::update()
{
	m_frame_resource_index = (m_frame_resource_index + 1) % frame_count;
	m_current_frame_resource = m_frame_resources[m_frame_resource_index].get();
	// Resources still scheduled for GPU execution cannot be modified or else undefined behavior will result.
	// Wait for the gpu to finish work for the previous frame resource.
	m_device_resources.command_queue->wait_for_fence_value(m_current_frame_resource->fence_value);

	//m_mvp_data = ...
}

void mvp_showcase_app::run()
{
	m_render_thread_handle = CreateThread(nullptr, 0, &mvp_showcase_app::record_cmd_lists, (void*)this, 0, nullptr);
}

void mvp_showcase_app::build_frame_resources()
{
	for (uint32_t i = 0; i < frame_count; ++i)
	{
		m_frame_resources.push_back(
			std::make_unique<frame_resource>(m_device_resources.device, nullptr)
		);
	}
}

void mvp_showcase_app::create_pso()
{
	D3D12_RENDER_TARGET_BLEND_DESC rt_blend_desc;
	rt_blend_desc.BlendEnable = false;
	rt_blend_desc.LogicOpEnable = false;
	rt_blend_desc.SrcBlend = D3D12_BLEND::D3D12_BLEND_ONE;
	rt_blend_desc.DestBlend = D3D12_BLEND::D3D12_BLEND_ZERO;
	rt_blend_desc.BlendOp = D3D12_BLEND_OP::D3D12_BLEND_OP_ADD;
	rt_blend_desc.SrcBlendAlpha = D3D12_BLEND::D3D12_BLEND_ONE;
	rt_blend_desc.DestBlendAlpha = D3D12_BLEND::D3D12_BLEND_ZERO;
	rt_blend_desc.BlendOpAlpha = D3D12_BLEND_OP::D3D12_BLEND_OP_ADD;
	rt_blend_desc.LogicOp = D3D12_LOGIC_OP::D3D12_LOGIC_OP_NOOP;
	rt_blend_desc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE::D3D12_COLOR_WRITE_ENABLE_ALL;

	D3D12_BLEND_DESC blend_state = {};
	blend_state.AlphaToCoverageEnable = false;
	blend_state.IndependentBlendEnable = false; // only blend the first RTV
	blend_state.RenderTarget[0] = rt_blend_desc;

	//Depth-stencil-state structure that controls how depth-stencil testing is performed by the output-merger stage.
	D3D12_DEPTH_STENCILOP_DESC depth_stencilop_desc;
	depth_stencilop_desc.StencilDepthFailOp = D3D12_STENCIL_OP::D3D12_STENCIL_OP_KEEP;
	depth_stencilop_desc.StencilFailOp = D3D12_STENCIL_OP::D3D12_STENCIL_OP_KEEP;
	depth_stencilop_desc.StencilPassOp = D3D12_STENCIL_OP::D3D12_STENCIL_OP_KEEP;
	depth_stencilop_desc.StencilFunc = D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_ALWAYS;

	D3D12_DEPTH_STENCIL_DESC depthstencil_desc;
	depthstencil_desc.DepthEnable = true;
	depthstencil_desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK::D3D12_DEPTH_WRITE_MASK_ALL;
	depthstencil_desc.DepthFunc = D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_LESS;
	depthstencil_desc.FrontFace = depth_stencilop_desc;
	depthstencil_desc.BackFace = depth_stencilop_desc;
	depthstencil_desc.StencilEnable = false;
	depthstencil_desc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
	depthstencil_desc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;

	D3D12_INPUT_ELEMENT_DESC input_elements[2] = {};
	D3D12_INPUT_ELEMENT_DESC positions_input_element;
	positions_input_element.SemanticName = "POSITION";
	positions_input_element.SemanticIndex = 0;
	positions_input_element.Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT;
	positions_input_element.InputSlot = 0;
	positions_input_element.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	positions_input_element.InputSlotClass = D3D12_INPUT_CLASSIFICATION::D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	positions_input_element.InstanceDataStepRate = 0;
	input_elements[0] = positions_input_element;

	D3D12_INPUT_ELEMENT_DESC colors_input_element;
	colors_input_element.SemanticName = "COLOR";
	colors_input_element.SemanticIndex = 0;
	colors_input_element.Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT;
	colors_input_element.InputSlot = 0;
	colors_input_element.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	colors_input_element.InputSlotClass = D3D12_INPUT_CLASSIFICATION::D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	colors_input_element.InstanceDataStepRate = 0;
	input_elements[1] = colors_input_element;

	D3D12_INPUT_LAYOUT_DESC input_layout;
	input_layout.NumElements = 2;
	input_layout.pInputElementDescs = input_elements;

	D3D12_RASTERIZER_DESC rasterizer_desc;
	rasterizer_desc.FillMode = D3D12_FILL_MODE::D3D12_FILL_MODE_SOLID;
	rasterizer_desc.CullMode = D3D12_CULL_MODE::D3D12_CULL_MODE_NONE;
	rasterizer_desc.FrontCounterClockwise = false;
	rasterizer_desc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	rasterizer_desc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	rasterizer_desc.DepthClipEnable = false;
	rasterizer_desc.MultisampleEnable = false;
	rasterizer_desc.AntialiasedLineEnable = false;
	rasterizer_desc.ForcedSampleCount = 0;
	rasterizer_desc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE::D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc = {};
	pso_desc.InputLayout = input_layout;
	pso_desc.pRootSignature = m_root_signature.get();
	pso_desc.BlendState = blend_state;
	pso_desc.RasterizerState = rasterizer_desc;
	pso_desc.DepthStencilState = depthstencil_desc;
	pso_desc.DSVFormat = DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
	pso_desc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE::D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
	pso_desc.NumRenderTargets = 1;
	pso_desc.RTVFormats[0] = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
	pso_desc.NodeMask = 0;
	pso_desc.Flags = D3D12_PIPELINE_STATE_FLAGS::D3D12_PIPELINE_STATE_FLAG_NONE;
	pso_desc.SampleDesc.Count = 1;
	pso_desc.SampleDesc.Quality = 0;
	pso_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//shaders
	D3D12_SHADER_BYTECODE vs_bytecode;
	vs_bytecode.BytecodeLength = m_shaders[L"default_shader.hlsl"]->GetBufferSize();
	vs_bytecode.pShaderBytecode = m_shaders[L"default_shader.hlsl"]->GetBufferPointer();
	pso_desc.VS = vs_bytecode;
	//pso_desc.DS
	//pso_desc.HS
	//pso_desc.GS
	//pso_desc.PS

	//pso_desc.StreamOutput
	//pso_desc.CachedPSO
	check_hresult(m_device_resources.device->CreateGraphicsPipelineState(&pso_desc, guid_of<ID3D12PipelineState>(), m_pso.put_void()));
}

void mvp_showcase_app::create_root_signature()
{
	D3D12_DESCRIPTOR_RANGE cb_manips_ranges[1];
	D3D12_DESCRIPTOR_RANGE sampler_ranges[1];
	D3D12_ROOT_PARAMETER params[2];

	D3D12_DESCRIPTOR_RANGE descriptor_range_cb;
	descriptor_range_cb.BaseShaderRegister = 0;
	descriptor_range_cb.NumDescriptors = 1;
	descriptor_range_cb.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//indicates this range should immediately follow the preceding range.
	descriptor_range_cb.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	descriptor_range_cb.RegisterSpace = 0;
	cb_manips_ranges[0] = descriptor_range_cb;

	D3D12_ROOT_DESCRIPTOR_TABLE cb_manips_table;
	cb_manips_table.NumDescriptorRanges = 1;
	cb_manips_table.pDescriptorRanges = cb_manips_ranges;

	D3D12_ROOT_PARAMETER cb_manips_param;
	cb_manips_param.ParameterType = D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	cb_manips_param.ShaderVisibility = D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_VERTEX;
	cb_manips_param.DescriptorTable = cb_manips_table;
	params[0] = cb_manips_param;

	// Samplers cannot be mixed with other resource types in a descriptor table (root parameter [0])
	D3D12_DESCRIPTOR_RANGE descriptor_range_sampler;
	descriptor_range_sampler.BaseShaderRegister = 0;
	descriptor_range_sampler.NumDescriptors = 1;
	descriptor_range_sampler.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//indicates this range should immediately follow the preceding range.
	descriptor_range_sampler.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
	descriptor_range_sampler.RegisterSpace = 0;
	sampler_ranges[0] = descriptor_range_sampler;

	D3D12_ROOT_DESCRIPTOR_TABLE sampler_table;
	sampler_table.NumDescriptorRanges = 1;
	sampler_table.pDescriptorRanges = sampler_ranges;

	D3D12_ROOT_PARAMETER root_param_sampler;
	root_param_sampler.ParameterType = D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	root_param_sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_VERTEX;
	root_param_sampler.DescriptorTable = sampler_table;
	params[1] = root_param_sampler;

	D3D12_ROOT_SIGNATURE_DESC rootsig_desc;
	rootsig_desc.Flags = D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootsig_desc.NumParameters = _countof(params);
	rootsig_desc.NumStaticSamplers = 0;
	rootsig_desc.pParameters = params;

	ID3DBlob* rootsig_blob = nullptr;
	ID3DBlob* error_blob = nullptr;

	hresult hr = D3D12SerializeRootSignature(&rootsig_desc, D3D_ROOT_SIGNATURE_VERSION::D3D_ROOT_SIGNATURE_VERSION_1, &rootsig_blob, &error_blob);
	if (error_blob != nullptr)
	{
		auto msg_ptr = static_cast<const char*>(error_blob->GetBufferPointer());
		hstring error_msg = winrt::to_hstring(msg_ptr);
	}
	check_hresult(hr);

	check_hresult(m_device_resources.device->CreateRootSignature(
		0,
		rootsig_blob->GetBufferPointer(),
		rootsig_blob->GetBufferSize(),
		guid_of<ID3D12RootSignature>(),
		m_root_signature.put_void()));
}

void mvp_showcase_app::create_mvp_cbv()
{
	//create upload buffer
	D3D12_HEAP_PROPERTIES upload_heap_props;
	upload_heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY::D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	upload_heap_props.CreationNodeMask = 0;
	upload_heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL::D3D12_MEMORY_POOL_UNKNOWN;
	upload_heap_props.Type = D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD;
	upload_heap_props.VisibleNodeMask = 0;

	D3D12_RESOURCE_DESC cbv_resource_desc;
	cbv_resource_desc.Alignment = 0;
	cbv_resource_desc.DepthOrArraySize = 1;
	cbv_resource_desc.Dimension = D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_BUFFER;
	cbv_resource_desc.Flags = D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE;
	cbv_resource_desc.Format = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
	cbv_resource_desc.Height = 1;
	// align to 256 bytes (minimum hardware allocation size)
	cbv_resource_desc.Width = (sizeof(object_constant_buffer) + 255) & ~255;
	//Layout must be D3D12_TEXTURE_LAYOUT_ROW_MAJOR when D3D12_RESOURCE_DESC::Dimension is D3D12_RESOURCE_DIMENSION_BUFFER
	cbv_resource_desc.Layout = D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	cbv_resource_desc.MipLevels = 1;
	cbv_resource_desc.SampleDesc.Count = 1;
	cbv_resource_desc.SampleDesc.Quality = 0;

	check_hresult(m_device_resources.device->CreateCommittedResource(
		&upload_heap_props,
		D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
		&cbv_resource_desc,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		guid_of<ID3D12Resource>(),
		m_cbv_uploaded_resource.put_void()));

	m_cbv_uploaded_resource->SetName(L"cbv_uploaded_resource");

	D3D12_RANGE range;
	range.Begin = 0;
	range.End = sizeof(object_constant_buffer);
	m_mvp_data = nullptr;
	m_cbv_uploaded_resource->Map(0, &range, reinterpret_cast<void**>(&m_mvp_data));

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_desc = {};
	cbv_desc.BufferLocation = m_cbv_uploaded_resource->GetGPUVirtualAddress();
	//Size of 64 is invalid.Device requires SizeInBytes be a multiple of 256
	cbv_desc.SizeInBytes = (sizeof(object_constant_buffer) + 255) & ~255;

	m_device_resources.device->CreateConstantBufferView(&cbv_desc, m_srv_cbv_uav_heap->GetCPUDescriptorHandleForHeapStart());
}

void mvp_showcase_app::create_srv_cbv_uav_heap(uint32_t descriptor_count)
{
	D3D12_DESCRIPTOR_HEAP_DESC srv_cbv_uav_heap_desc;
	srv_cbv_uav_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srv_cbv_uav_heap_desc.NodeMask = 0;
	srv_cbv_uav_heap_desc.NumDescriptors = descriptor_count;
	srv_cbv_uav_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	check_hresult(m_device_resources.device->CreateDescriptorHeap(&srv_cbv_uav_heap_desc, guid_of<ID3D12DescriptorHeap>(), m_srv_cbv_uav_heap.put_void()));
}

void mvp_showcase_app::create_cube()
{
	GeometryGenerator geo_gen;
	m_cube = geo_gen.CreateBox(1.5f, 1.f, 1.5f, 3);

	// upload vertex data to gpu through an upload buffer
	mesh cube_mesh;
	m_frame_resources[0]->cmd_list->Reset(m_frame_resources[0]->cmd_allocator.get(), nullptr);

	cube_mesh.upload_to_gpu(
		m_device_resources.device.get(),
		m_frame_resources[0]->cmd_list.get(),
		m_cube.Vertices.data(),
		m_cube.Vertices.size() * sizeof(GeometryGenerator::Vertex));
	// create vertex buffer view in the render_item
}
