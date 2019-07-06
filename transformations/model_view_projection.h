#pragma once

#include "model_view_projection.g.h"
#include "mvp_viewmodel.h"
#include "mvp_showcase_app.h"
#include "enum_to_bool.h"
#include "xaml_helpers.h"
#include "not_zero.h"
#include "mesh_vm.h"
#include <winrt/Windows.UI.Input.h>

namespace winrt::transformations::implementation
{
	struct model_view_projection : model_view_projectionT<model_view_projection>
	{
		model_view_projection();

		transformations::mvp_viewmodel vm();
		winrt::Windows::Foundation::IAsyncAction up_direction_checked(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
		winrt::Windows::Foundation::IAsyncAction focus_point_checked(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
		winrt::Windows::Foundation::IAsyncAction page_loaded(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
		winrt::Windows::Foundation::IAsyncAction page_size_changed(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
		winrt::Windows::Foundation::IAsyncAction swapchainpanel_onclick(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs const& args);

	private:
		mvp_showcase_app m_app;
		transformations::mvp_viewmodel m_vm = winrt::make<transformations::implementation::mvp_viewmodel>(&m_app);
	};
}

namespace winrt::transformations::factory_implementation
{
	struct model_view_projection : model_view_projectionT<model_view_projection, implementation::model_view_projection>
	{
	};
}
