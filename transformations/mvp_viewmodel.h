#pragma once
#include "mvp_viewmodel.g.h"
#include "bindable_base.h"
#include "mvp_showcase_app.h"

namespace winrt::transformations::implementation
{
	struct mvp_viewmodel : mvp_viewmodelT<mvp_viewmodel, transformations::implementation::bindable_base>
	{
		mvp_viewmodel();

		Windows::UI::Xaml::Input::XamlUICommand initialize_app();
		Windows::UI::Xaml::Controls::SwapChainPanel current_swapchain_panel();
		void current_swapchain_panel(Windows::UI::Xaml::Controls::SwapChainPanel value);

		float eye_position_x();
		void eye_position_x(float value);
		float eye_position_y();
		void eye_position_y(float value);
		float eye_position_z();
		void eye_position_z(float value);

		float angle();
		void angle(float value);
		float rotation_axis_x();
		void rotation_axis_x(float value);
		float rotation_axis_y();
		void rotation_axis_y(float value);
		float rotation_axis_z();
		void rotation_axis_z(float value);

		float viewport_width();
		void viewport_width(float value);
		float viewport_height();
		void viewport_height(float value);

		float scissor_rect_width();
		void scissor_rect_width(float value);
		float scissor_rect_height();
		void scissor_rect_height(float value);

		float field_of_view();
		void field_of_view(float value);
		float near_z();
		void near_z(float value);
		float far_z();
		void far_z(float value);

		void options_visible(bool value);
		bool options_visible();

	private:
		hstring m_title;
		Windows::UI::Xaml::Input::XamlUICommand m_initialize_app;
		Windows::UI::Xaml::Controls::SwapChainPanel m_current_swapchain_panel;
		winrt::Windows::Foundation::IAsyncAction initialize();

		float m_eye_position_x = 0.f;
		float m_eye_position_y = 0.f;
		float m_eye_position_z = -10.f;

		float m_angle = 0.0f;
		float m_rotation_axis_x = 0.0f;
		float m_rotation_axis_y = 1.0f;
		float m_rotation_axis_z = 1.0f;

		float m_viewport_width = 512;
		float m_viewport_height = 512;

		float m_scissor_rect_width = 512;
		float m_scissor_rect_height = 512;

		float m_field_of_view = 45.f;
		float m_near_z = 0.1f;
		float m_far_z = 100.f;
		bool m_options_visible = false;
	};
}
namespace winrt::transformations::factory_implementation
{
	struct mvp_viewmodel : mvp_viewmodelT<mvp_viewmodel, implementation::mvp_viewmodel>
	{
	};
}
