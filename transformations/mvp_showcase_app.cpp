#include "pch.h"
#include "mvp_showcase_app.h"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace DirectX;

mvp_showcase_app::mvp_showcase_app()
{
	m_system_info = system_info();
	m_device_resources = device_resources();
}

mvp_showcase_app::~mvp_showcase_app()
{
}

void mvp_showcase_app::compile_shaders()
{
	com_ptr<ID3DBlob> new_vertex_shader = nullptr;
	hstring file_name = L"default_shader.hlsl";
	hstring vs_shader_name = L"vs";
	hstring ps_shader_name = L"ps";
	auto shaders_folder = winrt::Windows::ApplicationModel::Package::Current().InstalledLocation().Path() + L"\\shaders\\";
	m_shaders[vs_shader_name] = m_device_resources.compile_shader_from_file(shaders_folder + file_name, "VS", "vs_5_1");
	m_shaders[ps_shader_name] = m_device_resources.compile_shader_from_file(shaders_folder + file_name, "PS", "ps_5_1");
}

void mvp_showcase_app::create_cmd_objects()
{
	m_cmd_queue = std::make_shared<cmd_queue>(m_device_resources.device);

	check_hresult(
		m_device_resources.device->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT,
			m_frame_resources[m_frame_resource_index]->cmd_allocator.get(),
			nullptr,
			guid_of<ID3D12GraphicsCommandList>(),
			m_graphics_cmdlist.put_void())
	);
}

bool mvp_showcase_app::initialize(transformations::mvp_viewmodel& vm)
{
#if defined(_DEBUG)
	debug_tools::enable_debug_layer(true);
	debug_tools::track_leaks_for_thread();
#endif

	if (!XMVerifyCPUSupport())
	{
		return false;
	}

	m_current_vm = vm;

	update_viewport();
	update_scissor_rect();

	m_output_width = m_current_vm.viewport_width();
	m_output_height = m_current_vm.viewport_height();

	com_ptr<IDXGIAdapter4> adapter = m_system_info.get_high_performance_adapter();

	if (adapter)
	{
		m_device_resources.create_device(adapter);
		build_frame_resources();
		create_cmd_objects();
		m_device_resources.create_dsv_heap();
		m_device_resources.create_dsv(m_output_width, static_cast<UINT>(m_output_height));
		m_device_resources.create_xaml_swapchain(m_cmd_queue, m_current_vm.current_swapchain_panel(), m_system_info.dxgi_factory, static_cast<UINT>(m_output_width), static_cast<UINT>(m_output_height));
		m_device_resources.create_rtv_heap();
		m_device_resources.create_rtv();
		m_device_resources.create_sampler_heap();
		m_device_resources.create_sampler();
		create_srv_cbv_uav_heap(1);
		create_root_signature();
	}
}

void mvp_showcase_app::load_content()
{
	compile_shaders();
	create_mvp_cbv();
	create_pso();
	create_cube();
	m_cmd_queue->execute_cmd_list(m_graphics_cmdlist);
	m_cmd_queue->flush_cmd_queue();
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

void mvp_showcase_app::update()
{
	m_current_frame_resource = m_frame_resources[m_frame_resource_index].get();

	new_width = m_current_vm.viewport_width();
	new_height = m_current_vm.viewport_height();

	if (new_width != old_width || new_height != old_height)
	{
		update_viewport();

		m_cmd_queue->flush_cmd_queue();
		m_device_resources.resize_rtv(static_cast<UINT>(new_width), static_cast<UINT>(new_height));
		m_current_backbuffer_index = 0;
		m_cmd_queue->flush_cmd_queue();
	}

	update_scissor_rect();

	update_mvp_matrix();

	old_width = new_width;
	old_height = new_height;
}

void mvp_showcase_app::render()
{
	check_hresult(m_current_frame_resource->cmd_allocator->Reset());
	check_hresult(m_graphics_cmdlist->Reset(m_current_frame_resource->cmd_allocator.get(), nullptr));

	//D3D12 WARNING: ID3D12CommandList::DrawInstanced: Viewport: 0 is non-empty while the corresponding scissor rectangle is empty.  
	//Nothing will be written to the render target when this viewport is selected.  In D3D12, scissor testing is always enabled. [ EXECUTION WARNING #695: DRAW_EMPTY_SCISSOR_RECTANGLE]
	m_graphics_cmdlist->RSSetScissorRects(1, &m_scissor_rect);
	m_graphics_cmdlist->RSSetViewports(1, &m_viewport);

	m_device_resources.transition_resource(m_graphics_cmdlist.get(), m_device_resources.get_render_target(m_current_backbuffer_index),
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET);

	D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle = m_device_resources.get_current_rtv(m_current_backbuffer_index);
	D3D12_CPU_DESCRIPTOR_HANDLE dsv_handle = m_device_resources.dsv_heap->GetCPUDescriptorHandleForHeapStart();
	m_graphics_cmdlist->ClearRenderTargetView(rtv_handle, Colors::BurlyWood, 0, nullptr);
	m_graphics_cmdlist->ClearDepthStencilView(m_device_resources.dsv_heap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	m_graphics_cmdlist->OMSetRenderTargets(1, &rtv_handle, true, &dsv_handle);

	m_graphics_cmdlist->SetGraphicsRootSignature(m_root_signature.get());

	std::array<ID3D12DescriptorHeap*, 2> descriptor_heaps = { m_srv_cbv_uav_heap.get(), m_device_resources.sampler_heap.get() };
	m_graphics_cmdlist->SetDescriptorHeaps(static_cast<UINT>(descriptor_heaps.size()), descriptor_heaps.data());

	// draw the cube 
	m_graphics_cmdlist->SetPipelineState(m_pso.get());
	m_graphics_cmdlist->IASetVertexBuffers(0, 1, &m_cube_mesh.vbv);
	m_graphics_cmdlist->IASetIndexBuffer(&m_cube_mesh.ibv);
	m_graphics_cmdlist->IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//m_graphics_cmdlist->SetGraphicsRootConstantBufferView(0, m_cbv_uploaded_resource->GetGPUVirtualAddress());
	m_graphics_cmdlist->SetGraphicsRootDescriptorTable(0, m_srv_cbv_uav_heap->GetGPUDescriptorHandleForHeapStart());
	m_graphics_cmdlist->DrawIndexedInstanced(m_cube_mesh.index_count, 1, 0, 0, 0);

	m_device_resources.transition_resource(m_graphics_cmdlist.get(), m_device_resources.get_render_target(m_current_backbuffer_index),
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT);

	m_cmd_queue->execute_cmd_list(m_graphics_cmdlist);
	m_current_backbuffer_index = m_device_resources.present();

	// next frame
	m_current_frame_resource->fence_value = m_cmd_queue->signal_here();
	m_cmd_queue->wait_for_fence_value(m_current_frame_resource->fence_value);
	m_frame_resource_index = (m_frame_resource_index + 1) % frame_count;
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
			std::make_unique<frame_resource>(m_device_resources.device)
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

	//D3D12_INPUT_ELEMENT_DESC input_layout[] = {
	//	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	//	{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	//};

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
	//pso_desc.InputLayout = { input_layout, _countof(input_layout) };
	pso_desc.pRootSignature = m_root_signature.get();
	pso_desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	pso_desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	pso_desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	//pso_desc.BlendState = blend_state;
	//pso_desc.RasterizerState = rasterizer_desc;
	//pso_desc.DepthStencilState = depthstencil_desc;
	pso_desc.DSVFormat = DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
	pso_desc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE::D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
	pso_desc.NumRenderTargets = 1;
	pso_desc.RTVFormats[0] = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
	pso_desc.NodeMask = 0;
	pso_desc.Flags = D3D12_PIPELINE_STATE_FLAGS::D3D12_PIPELINE_STATE_FLAG_NONE;
	pso_desc.SampleMask = UINT_MAX;
	pso_desc.SampleDesc.Count = 1;
	pso_desc.SampleDesc.Quality = 0;
	pso_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//shaders
	D3D12_SHADER_BYTECODE vs_bytecode;
	vs_bytecode.BytecodeLength = m_shaders[L"vs"]->GetBufferSize();
	vs_bytecode.pShaderBytecode = m_shaders[L"vs"]->GetBufferPointer();

	D3D12_SHADER_BYTECODE ps_bytecode;
	ps_bytecode.BytecodeLength = m_shaders[L"ps"]->GetBufferSize();
	ps_bytecode.pShaderBytecode = m_shaders[L"ps"]->GetBufferPointer();

	pso_desc.VS = vs_bytecode;
	pso_desc.PS = ps_bytecode;
	//pso_desc.DS
	//pso_desc.HS
	//pso_desc.GS

	//pso_desc.StreamOutput
	//pso_desc.CachedPSO
	check_hresult(m_device_resources.device->CreateGraphicsPipelineState(&pso_desc, guid_of<ID3D12PipelineState>(), m_pso.put_void()));
}

void mvp_showcase_app::update_viewport()
{
	m_viewport.TopLeftX = 0;
	m_viewport.TopLeftY = 0;
	m_viewport.Width = m_current_vm.viewport_width();
	m_viewport.Height = m_current_vm.viewport_height();
	m_viewport.MaxDepth = 1.0f;
	m_viewport.MinDepth = 0.f;
}

void mvp_showcase_app::update_scissor_rect()
{
	m_scissor_rect.top = 0;
	m_scissor_rect.left = 0;
	m_scissor_rect.right = static_cast<LONG>(m_current_vm.scissor_rect_width());
	m_scissor_rect.bottom = static_cast<LONG>(m_current_vm.scissor_rect_height());
}

void mvp_showcase_app::update_mvp_matrix()
{
	// model matrix
	const XMVECTOR rotation_axis = XMVectorSet(m_current_vm.rotation_axis_x(), m_current_vm.rotation_axis_y(), m_current_vm.rotation_axis_z(), 0);
	m_model = XMMatrixRotationAxis(rotation_axis, XMConvertToRadians(m_current_vm.angle()));

	// view matrix
	XMVECTOR eye_position = XMVectorSet(m_current_vm.eye_position_x(), m_current_vm.eye_position_y(), m_current_vm.eye_position_z(), 1);
	XMVECTOR focus_point = XMVectorSet(0, 0, 0, 1);
	XMVECTOR up_direction = XMVectorSet(0, 1, 0, 0);
	m_view = XMMatrixLookAtLH(eye_position, focus_point, up_direction);

	// projection matrix
	float aspect_ratio = m_output_width / static_cast<float>(m_output_height);
	m_projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(m_current_vm.field_of_view()), aspect_ratio, m_current_vm.near_z(), m_current_vm.far_z());

	// mvp matrix
	XMMATRIX mvp = XMMatrixMultiply(m_model, m_view);
	mvp = XMMatrixMultiply(mvp, m_projection);

	// upload
	XMStoreFloat4x4(&m_stored_mvp, mvp);
	memcpy(
		reinterpret_cast<void*>(m_mvp_data),
		reinterpret_cast<void*>(&m_stored_mvp),
		sizeof(object_constant_buffer));
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
	//m_mvp_data = nullptr;
	m_cbv_uploaded_resource->Map(0, &range, reinterpret_cast<void**>(&m_mvp_data));

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_desc = {};
	cbv_desc.BufferLocation = m_cbv_uploaded_resource->GetGPUVirtualAddress();
	//Size of 64 is invalid. Device requires SizeInBytes be a multiple of 256
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
	//GeometryGenerator geo_gen;
	//m_cube = geo_gen.CreateBox(1.5f, 1.f, 1.5f, 3);

	struct VertexPosColor
	{
		XMFLOAT3 Position;
		XMFLOAT3 Color;
	};

	static VertexPosColor g_Vertices[8] = {
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) }, // 0
		{ XMFLOAT3(-1.0f,  1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) }, // 1
		{ XMFLOAT3(1.0f,  1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, 0.0f) }, // 2
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) }, // 3
		{ XMFLOAT3(-1.0f, -1.0f,  1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) }, // 4
		{ XMFLOAT3(-1.0f,  1.0f,  1.0f), XMFLOAT3(0.0f, 1.0f, 1.0f) }, // 5
		{ XMFLOAT3(1.0f,  1.0f,  1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f) }, // 6
		{ XMFLOAT3(1.0f, -1.0f,  1.0f), XMFLOAT3(1.0f, 0.0f, 1.0f) }  // 7
	};

	static WORD g_Indicies[36] =
	{
		0, 1, 2, 0, 2, 3,
		4, 6, 5, 4, 7, 6,
		4, 5, 1, 4, 1, 0,
		3, 2, 6, 3, 6, 7,
		1, 5, 6, 1, 6, 2,
		4, 0, 3, 4, 3, 7
	};

	// vertex data
	size_t buffer_byte_stride = sizeof(VertexPosColor);
	size_t element_count = _countof(g_Vertices);
	size_t buffer_byte_size = buffer_byte_stride * element_count;

	m_device_resources.create_default_buffer(
		m_device_resources.device.get(), m_graphics_cmdlist.get(),
		&CD3DX12_RESOURCE_DESC::Buffer(buffer_byte_size), g_Vertices,
		m_cube_mesh.vertex_uploader.put(), m_cube_mesh.vertex_default.put());

	m_cube_mesh.vbv.BufferLocation = m_cube_mesh.vertex_default->GetGPUVirtualAddress();
	m_cube_mesh.vbv.SizeInBytes = buffer_byte_size;
	m_cube_mesh.vbv.StrideInBytes = buffer_byte_stride;
	m_cube_mesh.vertex_count = element_count;

	// index data
	element_count = _countof(g_Indicies);
	buffer_byte_stride = sizeof(WORD);
	buffer_byte_size = buffer_byte_stride * element_count;

	m_device_resources.create_default_buffer(
		m_device_resources.device.get(), m_graphics_cmdlist.get(),
		&CD3DX12_RESOURCE_DESC::Buffer(buffer_byte_size), g_Indicies,
		m_cube_mesh.index_uploader.put(), m_cube_mesh.index_default.put());

	m_cube_mesh.index_count = element_count;
	m_cube_mesh.ibv.BufferLocation = m_cube_mesh.index_default->GetGPUVirtualAddress();
	m_cube_mesh.ibv.Format = DXGI_FORMAT::DXGI_FORMAT_R16_UINT;
	m_cube_mesh.ibv.SizeInBytes = buffer_byte_size;
}
