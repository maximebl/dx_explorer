#pragma once
#include "mvp_viewmodel.g.h"
#include "bindable_base.h"
#include "mesh_vm.h"
#include "float3.h"

class mvp_showcase_app;

namespace winrt::transformations::implementation
{
	struct mvp_viewmodel : mvp_viewmodelT<mvp_viewmodel, transformations::implementation::bindable_base>
	{
		mvp_viewmodel(mvp_showcase_app* app);
		mvp_viewmodel() = default;

		transformations::mesh_vm selected_mesh();
		void selected_mesh(transformations::mesh_vm value);
		Windows::Foundation::Collections::IObservableVector<IInspectable> meshes();

		Windows::UI::Xaml::Input::XamlUICommand initialize_app();
		Windows::UI::Xaml::Input::XamlUICommand cmd_add_lit_cube();
		Windows::UI::Xaml::Input::XamlUICommand cmd_add_simple_cube();
		Windows::UI::Xaml::Input::XamlUICommand cmd_delete_mesh();
		Windows::Foundation::IAsyncAction pick(float screen_x, float screen_y);
		transformations::float3 clicked_viewport_position();
        void clicked_viewport_position(transformations::float3 value);

		Windows::UI::Xaml::Controls::SwapChainPanel current_swapchain_panel();
		void current_swapchain_panel(Windows::UI::Xaml::Controls::SwapChainPanel value);

		float eye_position_x();
		void eye_position_x(float value);
		float eye_position_y();
		void eye_position_y(float value);
		float eye_position_z();
		void eye_position_z(float value);
		transformations::vector_selection up_direction();
		void up_direction(transformations::vector_selection value);
		transformations::vector_selection focus_point();
		void focus_point(transformations::vector_selection value);

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

		void viewport_options_visible(bool value);
		bool viewport_options_visible();
		void model_matrix_options_visible(bool value);
		bool model_matrix_options_visible();
		void view_matrix_options_visible(bool value);
		bool view_matrix_options_visible();
		void projection_matrix_options_visible(bool value);
		bool projection_matrix_options_visible();
		void transforms_options_visible(bool value);
		bool transforms_options_visible();

	private:
		winrt::apartment_context m_ui_thread;

		transformations::mesh_vm m_selected_mesh = winrt::make<transformations::implementation::mesh_vm>();
		Windows::Foundation::Collections::IObservableVector<IInspectable> m_meshes = winrt::single_threaded_observable_vector<IInspectable>();
		std::unique_ptr<mvp_showcase_app> m_app = nullptr;

		Windows::UI::Xaml::Input::XamlUICommand m_initialize_app;
		Windows::UI::Xaml::Input::XamlUICommand m_cmd_add_lit_cube;
		Windows::UI::Xaml::Input::XamlUICommand m_cmd_add_simple_cube;
		Windows::UI::Xaml::Input::XamlUICommand m_cmd_delete_mesh;

		winrt::Windows::Foundation::IAsyncAction initialize();
		winrt::Windows::Foundation::IAsyncAction add_lit_cube();
		winrt::Windows::Foundation::IAsyncAction add_simple_cube();
		winrt::Windows::Foundation::IAsyncAction delete_mesh();

		hstring m_title;
		Windows::UI::Xaml::Controls::SwapChainPanel m_current_swapchain_panel;

		float m_eye_position_x = 0.f;
		float m_eye_position_y = 0.f;
		float m_eye_position_z = -10.f;
		transformations::vector_selection m_focus_point = transformations::vector_selection::w;

		float m_viewport_width = 512;
		float m_viewport_height = 512;

		float m_scissor_rect_width = 512;
		float m_scissor_rect_height = 512;

		float m_field_of_view = 45.f;
		float m_near_z = 0.1f;
		float m_far_z = 100.f;
		bool m_model_matrix_options_visible = true;
		bool m_view_matrix_options_visible = true;
		bool m_projection_matrix_options_visible = true;
		bool m_viewport_options_visible = true;
		bool m_transforms_options_visible = true;

		transformations::float3 m_clicked_viewport_pos = winrt::make<transformations::implementation::float3>(0.0f, 0.0f, 0.0f);
		transformations::vector_selection m_up_direction = transformations::vector_selection::y;
	};
}
namespace winrt::transformations::factory_implementation
{
	struct mvp_viewmodel : mvp_viewmodelT<mvp_viewmodel, implementation::mvp_viewmodel>
	{
	};
}
