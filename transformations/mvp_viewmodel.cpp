#include "pch.h"
#include "mvp_viewmodel.h"
#include "mvp_viewmodel.g.cpp"
#include "ui_helpers.h"
#include "mvp_showcase_app.h"

using namespace winrt::Windows::UI::Xaml::Input;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;

namespace winrt::transformations::implementation
{
	mvp_viewmodel::mvp_viewmodel(mvp_showcase_app* app)
	{
		m_ui_thread = winrt::apartment_context();

		m_app.reset(app);

		m_initialize_app.ExecuteRequested(
			[this](XamlUICommand sender, ExecuteRequestedEventArgs args) -> IAsyncAction {
			initialize();
			co_return;
		});

		m_cmd_add_lit_cube.ExecuteRequested(
			[this](XamlUICommand sender, ExecuteRequestedEventArgs args) -> IAsyncAction {
			add_lit_cube();
			co_return;
		});

		m_cmd_add_simple_cube.ExecuteRequested(
			[this](XamlUICommand sender, ExecuteRequestedEventArgs args) -> IAsyncAction {
			add_simple_cube();
			co_return;
		});
	}

	transformations::mesh_vm mvp_viewmodel::selected_mesh()
	{
		return m_selected_mesh;
	}

	void mvp_viewmodel::selected_mesh(transformations::mesh_vm value)
	{
		update_value(L"selected_mesh", m_selected_mesh, value);
	}

	IObservableVector<transformations::mesh_vm> mvp_viewmodel::meshes()
	{
		return m_meshes;
	}

	XamlUICommand mvp_viewmodel::initialize_app()
	{
		return m_initialize_app;
	}

	Windows::UI::Xaml::Input::XamlUICommand mvp_viewmodel::cmd_add_lit_cube()
	{
		return m_cmd_add_lit_cube;
	}

	Windows::UI::Xaml::Input::XamlUICommand mvp_viewmodel::cmd_add_simple_cube()
	{
		return m_cmd_add_simple_cube;
	}

	Windows::UI::Xaml::Controls::SwapChainPanel mvp_viewmodel::current_swapchain_panel()
	{
		return m_current_swapchain_panel;
	}

	void mvp_viewmodel::current_swapchain_panel(Windows::UI::Xaml::Controls::SwapChainPanel value)
	{
		m_current_swapchain_panel = value;
	}

	float mvp_viewmodel::eye_position_x()
	{
		return m_eye_position_x;
	}

	void mvp_viewmodel::eye_position_x(float value)
	{
		update_value(L"eye_position_x", m_eye_position_x, value);
	}

	float mvp_viewmodel::eye_position_y()
	{
		return m_eye_position_y;
	}

	void mvp_viewmodel::eye_position_y(float value)
	{
		update_value(L"eye_position_y", m_eye_position_y, value);
	}

	float mvp_viewmodel::eye_position_z()
	{
		return m_eye_position_z;
	}

	void mvp_viewmodel::eye_position_z(float value)
	{
		update_value(L"eye_position_z", m_eye_position_z, value);
	}

	float mvp_viewmodel::angle()
	{
		return m_angle;
	}

	void mvp_viewmodel::angle(float value)
	{
		update_value(L"angle", m_angle, value);
	}

	float mvp_viewmodel::rotation_axis_x()
	{
		return m_rotation_axis_x;
	}

	void mvp_viewmodel::rotation_axis_x(float value)
	{
		update_value(L"rotation_axis_x", m_rotation_axis_x, value);
	}

	float mvp_viewmodel::rotation_axis_y()
	{
		return m_rotation_axis_y;
	}

	void mvp_viewmodel::rotation_axis_y(float value)
	{
		update_value(L"rotation_axis_y", m_rotation_axis_y, value);
	}

	float mvp_viewmodel::rotation_axis_z()
	{
		return m_rotation_axis_z;
	}

	void mvp_viewmodel::rotation_axis_z(float value)
	{
		update_value(L"rotation_axis_z", m_rotation_axis_z, value);
	}

	float mvp_viewmodel::viewport_width()
	{
		return m_viewport_width;
	}

	void mvp_viewmodel::viewport_width(float value)
	{
		update_value(L"viewport_width", m_viewport_width, value);
	}

	float mvp_viewmodel::viewport_height()
	{
		return m_viewport_height;
	}

	void mvp_viewmodel::viewport_height(float value)
	{
		update_value(L"viewport_height", m_viewport_height, value);
	}

	float mvp_viewmodel::scissor_rect_width()
	{
		return m_scissor_rect_width;
	}

	void mvp_viewmodel::scissor_rect_width(float value)
	{
		update_value(L"scissor_rect_width", m_scissor_rect_width, value);
	}

	float mvp_viewmodel::scissor_rect_height()
	{
		return m_scissor_rect_height;
	}

	void mvp_viewmodel::scissor_rect_height(float value)
	{
		update_value(L"scissor_rect_height", m_scissor_rect_height, value);
	}

	float mvp_viewmodel::field_of_view()
	{
		return m_field_of_view;
	}

	void mvp_viewmodel::field_of_view(float value)
	{
		update_value(L"field_of_view", m_field_of_view, value);
	}

	float mvp_viewmodel::near_z()
	{
		return m_near_z;
	}

	void mvp_viewmodel::near_z(float value)
	{
		update_value(L"near_z", m_near_z, value);
	}

	float mvp_viewmodel::far_z()
	{
		return m_far_z;
	}

	void mvp_viewmodel::far_z(float value)
	{
		update_value(L"far_z", m_far_z, value);
	}

	void mvp_viewmodel::viewport_options_visible(bool value)
	{
		update_value(L"viewport_options_visible", m_viewport_options_visible, value);
	}

	bool mvp_viewmodel::viewport_options_visible()
	{
		return m_viewport_options_visible;
	}

	void mvp_viewmodel::model_matrix_options_visible(bool value)
	{
		update_value(L"model_matrix_options_visible", m_model_matrix_options_visible, value);
	}

	bool mvp_viewmodel::model_matrix_options_visible()
	{
		return m_model_matrix_options_visible;
	}

	void mvp_viewmodel::view_matrix_options_visible(bool value)
	{
		update_value(L"view_matrix_options_visible", m_view_matrix_options_visible, value);
	}

	bool mvp_viewmodel::view_matrix_options_visible()
	{
		return m_view_matrix_options_visible;
	}

	void mvp_viewmodel::projection_matrix_options_visible(bool value)
	{
		update_value(L"projection_matrix_options_visible", m_projection_matrix_options_visible, value);
	}

	bool mvp_viewmodel::projection_matrix_options_visible()
	{
		return m_projection_matrix_options_visible;
	}

	void mvp_viewmodel::transforms_options_visible(bool value)
	{
		update_value(L"transforms_options_visible", m_transforms_options_visible, value);
	}

	bool mvp_viewmodel::transforms_options_visible()
	{
		return m_transforms_options_visible;
	}

	transformations::vector_selection mvp_viewmodel::up_direction()
	{
		return m_up_direction;
	}

	void mvp_viewmodel::up_direction(transformations::vector_selection value)
	{
		update_value(L"up_direction", m_up_direction, value);
	}

	transformations::vector_selection mvp_viewmodel::focus_point()
	{
		return m_focus_point;
	}

	void mvp_viewmodel::focus_point(transformations::vector_selection value)
	{
		update_value(L"focus_point", m_focus_point, value);
	}

	IAsyncAction mvp_viewmodel::add_lit_cube()
	{
		m_app->create_lighting_cube();
		co_return;
	}

	IAsyncAction mvp_viewmodel::add_simple_cube()
	{
		transformations::mesh_vm new_mesh;
		new_mesh.as<transformations::implementation::mesh_vm>()->index = meshes().Size();
		selected_mesh(new_mesh.as<transformations::mesh_vm>());
		meshes().Append(new_mesh);

		co_await winrt::resume_background();
		m_app->create_cube();
		co_return;
	}

	IAsyncAction mvp_viewmodel::initialize()
	{
		transformations::mvp_viewmodel new_vm = *this;
		m_app->initialize(new_vm);
		m_app->load_content();
		m_app->run();
		co_return;
	}
}
