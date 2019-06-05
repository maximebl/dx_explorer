#pragma once
#include "debug_tools.h"
#include "system_info.h"
#include "device_resources.h"
#include "cmd_queue.h"
#include "frame_resource.h"
#include "../external_libs/GeometryGenerator.h"
#include "mesh.h"
#include "mvp_viewmodel.h"

class mvp_showcase_app
{
public:
	~mvp_showcase_app();

	mvp_showcase_app(const mvp_showcase_app&) = delete;
	mvp_showcase_app& operator=(const mvp_showcase_app &) = delete;
	mvp_showcase_app(mvp_showcase_app &&) = delete;
	mvp_showcase_app& operator=(mvp_showcase_app &&) = delete;

	bool initialize(transformations::mvp_viewmodel& vm);
	void load_content();
	void run();

	void create_root_signature();
	void create_mvp_cbv();
	void create_srv_cbv_uav_heap(uint32_t descriptor_count);
	void render();
	void update();
	void build_frame_resources();
	void create_pso();

	static DWORD __stdcall record_cmd_lists(void* instance);

	static mvp_showcase_app& get_instance()
	{
		static mvp_showcase_app s_app;
		return s_app;
	}

	std::vector<std::unique_ptr<frame_resource>> m_frame_resources;
	frame_resource* m_current_frame_resource;
	uint32_t m_frame_resource_index = 0;
	uint32_t m_current_backbuffer_index = 0;
	const uint32_t frame_count = 3;

	struct object_constant_buffer
	{
		DirectX::XMFLOAT4X4 model_view_projection;
	};

private:
	mvp_showcase_app();

	transformations::mvp_viewmodel m_current_vm;

	D3D12_VIEWPORT m_viewport;
	D3D12_RECT m_scissor_rect;
	uint64_t m_output_width = 0;
	uint64_t m_output_height = 0;

	uint32_t new_width = 0;
	uint32_t new_height = 0;
	uint32_t old_width = 0;
	uint32_t old_height = 0;

	void compile_shaders();
	void create_cmd_objects();
	void create_cube();

	void update_viewport();
	void update_scissor_rect();
	void update_mvp_matrix();

	GeometryGenerator::MeshData m_cube;
	system_info m_system_info;
	device_resources m_device_resources;

	std::shared_ptr<cmd_queue> m_cmd_queue = nullptr;
	winrt::com_ptr<ID3D12GraphicsCommandList4> m_graphics_cmdlist = nullptr;

	winrt::com_ptr<ID3D12RootSignature> m_root_signature = nullptr;
	winrt::com_ptr<ID3D12DescriptorHeap> m_srv_cbv_uav_heap = nullptr;
	winrt::com_ptr<ID3D12Resource> m_cbv_uploaded_resource = nullptr;
	winrt::com_ptr<ID3D12PipelineState> m_pso = nullptr;
	HANDLE m_render_thread_handle;
	std::unordered_map<winrt::hstring, winrt::com_ptr<ID3DBlob>> m_shaders;

	mesh m_cube_mesh;

	std::byte* m_mvp_data;
	DirectX::XMFLOAT4X4 m_stored_mvp;
    DirectX::XMMATRIX m_model;
    DirectX::XMMATRIX m_view;
    DirectX::XMMATRIX m_projection;
};

