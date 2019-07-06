#include "pch.h"
#include "mvp_showcase_app.h"
#include "math_helpers.h"

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
	m_shaders[L"vs"] = m_device_resources.default_shaders[L"vs"];
	m_shaders[L"ps"] = m_device_resources.default_shaders[L"ps"];
}

void mvp_showcase_app::create_cmd_objects()
{
	m_cmd_queue = std::make_shared<cmd_queue>(m_device_resources.device);

	check_hresult(m_device_resources.device->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT,
		m_frame_resources[m_frame_resource_index]->cmd_allocator.get(),
		nullptr,
		guid_of<ID3D12GraphicsCommandList>(),
		m_graphics_cmdlist.put_void()));
	check_hresult(m_graphics_cmdlist->SetName(L"graphics_cmdlist"));

	check_hresult(m_device_resources.device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT,
		guid_of<ID3D12CommandAllocator>(),
		ui_requests_cmd_allocator.put_void()));
	check_hresult(ui_requests_cmd_allocator->SetName(L"ui_requests_allocator"));

	check_hresult(m_device_resources.device->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT,
		ui_requests_cmd_allocator.get(),
		nullptr,
		guid_of<ID3D12GraphicsCommandList>(),
		m_ui_requests_cmdlist.put_void()));
	check_hresult(m_ui_requests_cmdlist->SetName(L"ui_requests_cmdlist"));
	check_hresult(m_ui_requests_cmdlist->Close());
}

bool mvp_showcase_app::initialize(transformations::mvp_viewmodel& vm)
{
#if defined(_DEBUG)
	m_device_resources.debug_tools.enable_debug_layer(debug_tools::debug_mode::enable_gpu_validation);
	m_device_resources.debug_tools.track_leaks_for_thread();
#endif

	if (!XMVerifyCPUSupport())
	{
		return false;
	}

	m_current_vm = vm;

	update_viewport();
	update_scissor_rect();

	m_output_width = m_current_vm.current_swapchain_panel().ActualWidth();
	m_output_height = m_current_vm.current_swapchain_panel().ActualHeight();
	m_current_vm.viewport_width(m_output_width);
	m_current_vm.viewport_height(m_output_height);
	m_current_vm.scissor_rect_width(m_output_width);
	m_current_vm.scissor_rect_height(m_output_height);

	com_ptr<IDXGIAdapter4> adapter = m_system_info.get_high_performance_adapter();

	if (adapter)
	{
		m_device_resources.create_device(adapter);

		m_device_resources.compile_default_shaders();
		m_device_resources.create_default_rootsig();
		m_device_resources.create_line_pso();

		build_frame_resources();
		create_cmd_objects();
		m_device_resources.create_dsv_heap();
		m_device_resources.create_dsv(static_cast<UINT64>(m_output_width), static_cast<UINT>(m_output_height));
		m_device_resources.create_xaml_swapchain(m_cmd_queue, m_current_vm.current_swapchain_panel(), m_system_info.dxgi_factory, static_cast<UINT>(m_output_width), static_cast<UINT>(m_output_height));
		m_device_resources.create_rtv_heap();
		m_device_resources.create_rtv();
		m_device_resources.create_sampler_heap();
		m_device_resources.create_sampler();
		create_srv_cbv_uav_heap(20);
		create_root_signature();
	}
}

void mvp_showcase_app::load_content()
{
	compile_shaders();
	create_view_proj_cbv();
	create_pso();
	m_cmd_queue->execute_cmd_list(m_graphics_cmdlist);
	m_cmd_queue->flush_cmd_queue();
}

void mvp_showcase_app::run()
{
	m_render_thread_handle = CreateThread(nullptr, 0, &mvp_showcase_app::render_loop, (void*)this, 0, nullptr);
}

void mvp_showcase_app::create_cmd_record_thread()
{
	m_cmd_recording_thread_handle = CreateThread(nullptr, 0, &mvp_showcase_app::record_cmd_lists, (void*)this, 0, nullptr);
}

void mvp_showcase_app::pick(float screen_x, float screen_y)
{
	// Picking ray in view space
	auto p00 = m_stored_mvp.projection(0, 0);
	auto p11 = m_stored_mvp.projection(1, 1);

	float view_x = (+2.0f * screen_x / m_current_vm.viewport_width() - 1.0f) / p00;
	float view_y = (-2.0f * screen_y / m_current_vm.viewport_height() + 1.0f) / p11;

	XMVECTOR tmp_origin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR tmp_direction = XMVectorSet(view_x, view_y, 1.0f, 0.0f);

	for (size_t i = 0; i < render_items.size(); ++i)
	{
		XMFLOAT4X4* model_world = &render_items[i].mesh_geometry.world_matrix;
		XMMATRIX world_matrix = XMMatrixTranspose(XMLoadFloat4x4(model_world));
		XMMATRIX inverted_world_matrix = XMMatrixInverse(&XMMatrixDeterminant(world_matrix), world_matrix);

		// transform picking ray to view space of mesh
		XMMATRIX tmp_view = XMLoadFloat4x4(&m_stored_mvp.view);
		tmp_view = XMMatrixTranspose(tmp_view);

		XMMATRIX inverted_view_matrix = XMMatrixInverse(&XMMatrixDeterminant(tmp_view), tmp_view);
		XMMATRIX to_local = XMMatrixMultiply(inverted_view_matrix, inverted_world_matrix);

		//Transforms the 3D vector normal by the given matrix.
		tmp_direction = XMVector3TransformNormal(tmp_direction, to_local);
		m_ray_direction = XMVector3Normalize(tmp_direction);

		//Transforms the 3D vector by a given matrix, projecting the result back into w = 1
		m_ray_origin = XMVector3TransformCoord(tmp_origin, to_local);
	}
}

DWORD __stdcall mvp_showcase_app::record_cmd_lists(void* instance)
{
	return 0;
}

DWORD __stdcall mvp_showcase_app::render_loop(void* instance)
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

	bool is_dimension_changed = (new_width != old_width || new_height != old_height);

	if (is_dimension_changed && new_width > 0 && new_height > 0)
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

	m_graphics_cmdlist->SetPipelineState(m_pso.get());
	m_graphics_cmdlist->SetGraphicsRootConstantBufferView(2, m_cbv_uploaded_resource->GetGPUVirtualAddress());

	// draw the cubes
	draw_render_items();

	vertex_pos_color v1;
	vertex_pos_color v2;
	XMStoreFloat3(&v1.Position, m_ray_origin);
	XMStoreFloat3(&v1.Color, DirectX::Colors::Red);

	XMStoreFloat3(&v2.Position, m_ray_direction);
	XMStoreFloat3(&v2.Color, DirectX::Colors::Red);

	pick(m_current_vm.clicked_viewport_position().x(), m_current_vm.clicked_viewport_position().y());
	draw_line(m_graphics_cmdlist.get(), v1, v2);

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

void mvp_showcase_app::build_frame_resources()
{
	for (uint32_t i = 0; i < frame_count; ++i)
	{
		m_frame_resources.push_back(std::make_unique<frame_resource>(m_device_resources.device));

		std::wstring current_id = std::to_wstring(i);
		std::wstring alloc_name = L"graphics_allocator_" + current_id;
		check_hresult(m_frame_resources[i]->cmd_allocator->SetName(alloc_name.c_str()));
	}
}

void mvp_showcase_app::create_pso()
{
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

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc = {};
	pso_desc.InputLayout = input_layout;
	pso_desc.pRootSignature = m_root_signature.get();
	pso_desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	pso_desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	pso_desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
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

	check_hresult(m_device_resources.device->CreateGraphicsPipelineState(&pso_desc, guid_of<ID3D12PipelineState>(), m_pso.put_void()));
}

void mvp_showcase_app::draw_render_items()
{
	for (int i = 0; i < render_items.size(); ++i)
	{
		//debug_tools::assert_resource_state(render_items[i].mesh_geometry.index_default.get(), D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_INDEX_BUFFER);
		//debug_tools::assert_resource_state(render_items[i].mesh_geometry.vertex_default.get(), D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		render_item& current_render_item = render_items[i];

		m_graphics_cmdlist->IASetVertexBuffers(0, 1, &current_render_item.mesh_geometry.vbv);
		m_graphics_cmdlist->IASetIndexBuffer(&current_render_item.mesh_geometry.ibv);
		m_graphics_cmdlist->IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_graphics_cmdlist->SetGraphicsRootDescriptorTable(0, current_render_item.cbv_gpu_handle);
		m_graphics_cmdlist->DrawIndexedInstanced(current_render_item.mesh_geometry.index_count, 1, 0, 0, 0);
	}
}

void mvp_showcase_app::draw_line(ID3D12GraphicsCommandList4* cmd_list, vertex_pos_color v1, vertex_pos_color v2)
{
	if (!line_data_uploaded)
	{
		line_ia_vertex_input = std::make_unique<upload_buffer<vertex_pos_color>>(
			m_device_resources.device.get(),
			2,
			upload_buffer<vertex_pos_color>::buffer_type::constant_buffer);

		line_data_uploaded = true;
	}
	else
	{
		line_ia_vertex_input->copy_data(0, v1);
		line_ia_vertex_input->copy_data(1, v2);

		line_vbv.BufferLocation = line_ia_vertex_input->upload_buffer_resource->GetGPUVirtualAddress();
		line_vbv.StrideInBytes = line_ia_vertex_input->element_size;
		line_vbv.SizeInBytes = line_ia_vertex_input->element_count * line_ia_vertex_input->element_size;

		cmd_list->SetGraphicsRootSignature(m_device_resources.default_root_signature.get());
		cmd_list->SetPipelineState(m_device_resources.default_psos[L"line"].get());
		cmd_list->IASetVertexBuffers(0, 1, &line_vbv);
		cmd_list->IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_LINELIST);
		cmd_list->DrawInstanced(2, 1, 0, 0);
	}
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
	update_model_matrices();

	// view matrix
	XMVECTOR eye_position = XMVectorSet(m_current_vm.eye_position_x(), m_current_vm.eye_position_y(), m_current_vm.eye_position_z(), 1);
	XMVECTOR focus_point;
	switch (m_current_vm.focus_point())
	{
	case transformations::vector_selection::x:
		focus_point = XMVectorSet(1, 0, 0, 0);
		break;
	case transformations::vector_selection::y:
		focus_point = XMVectorSet(0, 1, 0, 0);
		break;
	case transformations::vector_selection::z:
		focus_point = XMVectorSet(0, 0, 1, 0);
		break;
	case transformations::vector_selection::w:
		focus_point = XMVectorSet(0, 0, 0, 1);
		break;
	default:
		focus_point = XMVectorSet(0, 0, 0, 1);
		break;
	}

	XMVECTOR up_direction;
	switch (m_current_vm.up_direction())
	{
	case transformations::vector_selection::x:
		up_direction = XMVectorSet(1, 0, 0, 0);
		break;
	case transformations::vector_selection::y:
		up_direction = XMVectorSet(0, 1, 0, 0);
		break;
	case transformations::vector_selection::z:
		up_direction = XMVectorSet(0, 0, 1, 0);
		break;
	default:
		up_direction = XMVectorSet(0, 1, 0, 0);
		break;
	}

	//m_view = XMMatrixLookAtLH(eye_position, focus_point, up_direction);
	m_view = XMMatrixSet(
		0.948683321f, -0.0589483120f, -0.310684890f, 0.0f,
		-3.72529030e-09, 0.982471883f, -0.186410919f, 0.0f,
		0.316227794f, 0.176844940f, 0.932054639f, 0.0f,
		-0.0f, -0.982471704f, 16.2798882f, 1.0f);

	// projection matrix
	float aspect_ratio = m_current_vm.viewport_width() / m_current_vm.viewport_height();
	m_projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(m_current_vm.field_of_view()), aspect_ratio, m_current_vm.near_z(), m_current_vm.far_z());

	// upload
	XMStoreFloat4x4(&m_stored_mvp.view, XMMatrixTranspose(m_view));
	XMStoreFloat4x4(&m_stored_mvp.projection, XMMatrixTranspose(m_projection));

	memcpy(
		reinterpret_cast<void*>(m_mvp_data),
		reinterpret_cast<void*>(&m_stored_mvp),
		sizeof(view_proj_cb));
}

void mvp_showcase_app::update_model_matrices()
{
	auto current_mesh = m_current_vm.selected_mesh();
	if (current_mesh && render_items.size() > 0)
	{
		// model matrix
#if defined(DEBUG) || defined(_DEBUG)  
		bool axis_x = math_helpers::is_nearly_zero(current_mesh.rotation_axis().x());
		bool axis_y = math_helpers::is_nearly_zero(current_mesh.rotation_axis().y());
		bool axis_z = math_helpers::is_nearly_zero(current_mesh.rotation_axis().z());
		bool is_all_zeros = axis_x && axis_y && axis_z;
		_ASSERT_EXPR(!is_all_zeros, L"At least one rotation axis is required.");
#endif

		XMVECTOR rotation_axis = XMVectorSet(current_mesh.rotation_axis().x(), current_mesh.rotation_axis().y(), current_mesh.rotation_axis().z(), 0);
		XMMATRIX rotation_matrix = XMMatrixRotationAxis(rotation_axis, XMConvertToRadians(current_mesh.angle()));
		//XMMATRIX translation_matrix = XMMatrixTranslation(current_mesh.translation().x(), current_mesh.translation().y(), current_mesh.translation().z());
		XMMATRIX translation_matrix = XMMatrixTranslation(0.0f, 1.0f, 0.0f);
		XMMATRIX scaling_matrix = XMMatrixScaling(current_mesh.scale().x(), current_mesh.scale().y(), current_mesh.scale().z());

		m_current_index = current_mesh.index();

		m_model = rotation_matrix * translation_matrix * scaling_matrix;
		XMFLOAT4X4* model_world_matrix = &render_items[m_current_index].mesh_geometry.world_matrix;
		XMStoreFloat4x4(model_world_matrix, XMMatrixTranspose(m_model));

		if (render_items.size() > m_current_index)
		{
			memcpy(reinterpret_cast<void*>(render_items[m_current_index].constant_buffer_allocation.CPU),
				reinterpret_cast<void*>(model_world_matrix),
				sizeof(model_cb));
		}
	}
}

void mvp_showcase_app::create_root_signature()
{
	D3D12_DESCRIPTOR_RANGE cb_manips_ranges[1];
	D3D12_DESCRIPTOR_RANGE sampler_ranges[1];
	D3D12_ROOT_PARAMETER params[3];

	D3D12_DESCRIPTOR_RANGE descriptor_range_cb;
	descriptor_range_cb.BaseShaderRegister = 1;
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

	D3D12_ROOT_DESCRIPTOR view_proj_cb_root_descriptor;
	view_proj_cb_root_descriptor.RegisterSpace = 0;
	view_proj_cb_root_descriptor.ShaderRegister = 0;

	D3D12_ROOT_PARAMETER cb_view_proj_param;
	cb_view_proj_param.ParameterType = D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_CBV;
	cb_view_proj_param.ShaderVisibility = D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_VERTEX;
	cb_view_proj_param.Descriptor = view_proj_cb_root_descriptor;
	params[2] = cb_view_proj_param;

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

void mvp_showcase_app::create_view_proj_cbv()
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
	cbv_resource_desc.Width = (sizeof(view_proj_cb) + 255) & ~255;
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
	range.End = sizeof(view_proj_cb);
	m_cbv_uploaded_resource->Map(0, &range, reinterpret_cast<void**>(&m_mvp_data));

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_desc = {};
	cbv_desc.BufferLocation = m_cbv_uploaded_resource->GetGPUVirtualAddress();
	//Size of 64 is invalid. Device requires SizeInBytes be a multiple of 256
	cbv_desc.SizeInBytes = (sizeof(view_proj_cb) + 255) & ~255;

	m_device_resources.device->CreateConstantBufferView(&cbv_desc, m_srv_cbv_uav_heap->GetCPUDescriptorHandleForHeapStart());
}

void mvp_showcase_app::create_model_cbv(render_item& render_item)
{
	//render_item& render_item = render_items.back();

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
	cbv_resource_desc.Width = (sizeof(model_cb) + 255) & ~255;
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
		render_item.model_cbv_uploader.put_void()));

	D3D12_RANGE range;
	range.Begin = 0;
	range.End = sizeof(model_cb);
	render_item.model_cbv_uploader->Map(0, &range, &render_item.constant_buffer_allocation.CPU);
	render_item.constant_buffer_allocation.GPU = render_item.model_cbv_uploader->GetGPUVirtualAddress();

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_desc;
	cbv_desc.BufferLocation = render_item.constant_buffer_allocation.GPU;
	//Size of 64 is invalid. Device requires SizeInBytes be a multiple of 256
	cbv_desc.SizeInBytes = (sizeof(model_cb) + 255) & ~255;

	// We don't have to do -1 on the size of render_items because
	// the view-projection matrix data already takes the first index in the cbv descriptor table, so we start at index 1 
	UINT heap_index = render_items.size();

	D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle = m_srv_cbv_uav_heap->GetCPUDescriptorHandleForHeapStart();
	cpu_handle.ptr += m_device_resources.m_cbv_srv_uav_increment_size * heap_index;
	//render_item.cbv_cpu_handle = cpu_handle;

	D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle = m_srv_cbv_uav_heap->GetGPUDescriptorHandleForHeapStart();
	gpu_handle.ptr += m_device_resources.m_cbv_srv_uav_increment_size * heap_index;
	render_item.cbv_gpu_handle = gpu_handle;

	m_device_resources.device->CreateConstantBufferView(&cbv_desc, cpu_handle);
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

render_item mvp_showcase_app::create_simple_cube(ID3D12GraphicsCommandList4* cmd_list)
{
	static vertex_pos_color g_Vertices[8] = {
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) }, // 0
		{ XMFLOAT3(-1.0f,  1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) }, // 1
		{ XMFLOAT3(1.0f,  1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, 0.0f) }, // 2
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) }, // 3
		{ XMFLOAT3(-1.0f, -1.0f,  1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) }, // 4
		{ XMFLOAT3(-1.0f,  1.0f,  1.0f), XMFLOAT3(0.0f, 1.0f, 1.0f) }, // 5
		{ XMFLOAT3(1.0f,  1.0f,  1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f) }, // 6
		{ XMFLOAT3(1.0f, -1.0f,  1.0f), XMFLOAT3(1.0f, 0.0f, 1.0f) }  // 7
	};

	// vertex data
	size_t buffer_byte_stride = sizeof(vertex_pos_color);
	size_t element_count = _countof(g_Vertices);
	size_t buffer_byte_size = buffer_byte_stride * element_count;

	mesh m_cube_mesh;

	auto vertices_buffer = CD3DX12_RESOURCE_DESC::Buffer(buffer_byte_size);
	m_device_resources.create_default_buffer(
		cmd_list,
		&vertices_buffer, g_Vertices,
		m_cube_mesh.vertex_uploader.put(), m_cube_mesh.vertex_default.put(), L"vertex_buffer");

	m_cube_mesh.vbv.BufferLocation = m_cube_mesh.vertex_default->GetGPUVirtualAddress();
	m_cube_mesh.vbv.SizeInBytes = static_cast<UINT>(buffer_byte_size);
	m_cube_mesh.vbv.StrideInBytes = static_cast<UINT>(buffer_byte_stride);
	m_cube_mesh.vertex_count = static_cast<UINT>(element_count);

	// index data
	static WORD g_Indicies[36] =
	{
		0, 1, 2, 0, 2, 3,
		4, 6, 5, 4, 7, 6,
		4, 5, 1, 4, 1, 0,
		3, 2, 6, 3, 6, 7,
		1, 5, 6, 1, 6, 2,
		4, 0, 3, 4, 3, 7
	};

	element_count = _countof(g_Indicies);
	buffer_byte_stride = sizeof(WORD);
	buffer_byte_size = buffer_byte_stride * element_count;

	auto indices_buffer = CD3DX12_RESOURCE_DESC::Buffer(buffer_byte_size);
	m_device_resources.create_default_buffer(
		cmd_list,
		&indices_buffer, g_Indicies,
		m_cube_mesh.index_uploader.put(), m_cube_mesh.index_default.put(), L"index_buffer");

	m_cube_mesh.index_count = static_cast<UINT>(element_count);
	m_cube_mesh.ibv.BufferLocation = m_cube_mesh.index_default->GetGPUVirtualAddress();
	m_cube_mesh.ibv.Format = DXGI_FORMAT::DXGI_FORMAT_R16_UINT;
	m_cube_mesh.ibv.SizeInBytes = static_cast<UINT>(buffer_byte_size);

	render_item cube_render_item;
	cube_render_item.mesh_geometry = m_cube_mesh;
	return cube_render_item;
}

void mvp_showcase_app::create_cube()
{
	check_hresult(ui_requests_cmd_allocator->Reset());
	check_hresult(m_ui_requests_cmdlist->Reset(ui_requests_cmd_allocator.get(), nullptr));

	render_item render_item = create_simple_cube(m_ui_requests_cmdlist.get());
	create_model_cbv(render_item);
	render_items.push_back(std::move(render_item));

	m_cmd_queue->execute_cmd_list(m_ui_requests_cmdlist);
}

void mvp_showcase_app::create_lighting_cube()
{
	GeometryGenerator geo_gen;
	GeometryGenerator::MeshData lighting_cube = geo_gen.CreateBox(1.5f, 1.f, 1.5f, 3);

	// create the vertex buffer
	size_t buffer_byte_stride = sizeof(GeometryGenerator::Vertex);
	size_t buffer_byte_size = buffer_byte_stride * lighting_cube.Vertices.size();

	auto vertices_buffer = CD3DX12_RESOURCE_DESC::Buffer(buffer_byte_size);

	mesh m_lighting_cube_mesh;
	m_device_resources.create_default_buffer(
		m_graphics_cmdlist.get(),
		&vertices_buffer, lighting_cube.Vertices.data(),
		m_lighting_cube_mesh.vertex_uploader.put(), m_lighting_cube_mesh.vertex_default.put(), L"lit_cube_vertex_buffer");

	m_lighting_cube_mesh.vbv.BufferLocation = m_lighting_cube_mesh.vertex_default->GetGPUVirtualAddress();
	m_lighting_cube_mesh.vbv.SizeInBytes = buffer_byte_size;
	m_lighting_cube_mesh.vbv.StrideInBytes = buffer_byte_stride;

	// create the index buffer
	buffer_byte_stride = sizeof(uint32_t);
	buffer_byte_size = buffer_byte_stride * lighting_cube.Indices32.size();

	auto indices_buffer = CD3DX12_RESOURCE_DESC::Buffer(buffer_byte_size);

	m_device_resources.create_default_buffer(
		m_graphics_cmdlist.get(),
		&indices_buffer, lighting_cube.Indices32.data(),
		m_lighting_cube_mesh.index_uploader.put(), m_lighting_cube_mesh.index_default.put(), L"lit_cube_index_buffer");

	m_lighting_cube_mesh.ibv.BufferLocation = m_lighting_cube_mesh.index_default->GetGPUVirtualAddress();
	m_lighting_cube_mesh.ibv.Format = DXGI_FORMAT::DXGI_FORMAT_R16_UINT;
	m_lighting_cube_mesh.ibv.SizeInBytes = buffer_byte_size;
}
