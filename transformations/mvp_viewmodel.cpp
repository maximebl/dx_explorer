#include "pch.h"
#include "mvp_viewmodel.h"
#include "mvp_viewmodel.g.cpp"
#include "ui_helpers.h"

using namespace winrt::Windows::UI::Xaml::Input;
using namespace winrt::Windows::Foundation;

namespace winrt::transformations::implementation
{
	mvp_viewmodel::mvp_viewmodel()
	{
		m_initialize_app.ExecuteRequested(
			[this](XamlUICommand sender, ExecuteRequestedEventArgs args) -> IAsyncAction {
			initialize();
			co_return;
		});
	}

	XamlUICommand mvp_viewmodel::initialize_app()
	{
		return m_initialize_app;
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

	IAsyncAction mvp_viewmodel::initialize()
	{
		//if (!app.initialize(m_current_swapchain_panel, 512, 512))
		//{
		//	co_await ui_helpers::show_error_dialog(L"Failed to verify DirectX Math library support.", L"CPU not supported");
		//	co_return;
		//}
		//app.load_content();
		//co_await app.run();
		co_return;
	}
}
