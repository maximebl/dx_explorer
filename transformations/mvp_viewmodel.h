#pragma once
#include "mvp_viewmodel.g.h"
#include "bindable_base.h"
#include "mvp_showcase_app.h"

namespace winrt::transformations::implementation
{
	struct mvp_viewmodel : mvp_viewmodelT<mvp_viewmodel, transformations::implementation::bindable_base>
	{
		mvp_viewmodel();

		hstring Title();
		void Title(hstring value);

		Windows::UI::Xaml::Input::XamlUICommand initialize_app();
		Windows::UI::Xaml::Controls::SwapChainPanel current_swapchain_panel();
		void current_swapchain_panel(Windows::UI::Xaml::Controls::SwapChainPanel value);

	private:
		hstring m_title;
		Windows::UI::Xaml::Input::XamlUICommand m_initialize_app;
		Windows::UI::Xaml::Controls::SwapChainPanel m_current_swapchain_panel;
		winrt::Windows::Foundation::IAsyncAction initialize();

		//mvp_showcase_app& app = mvp_showcase_app::get_instance();
	};
}
namespace winrt::transformations::factory_implementation
{
	struct mvp_viewmodel : mvp_viewmodelT<mvp_viewmodel, implementation::mvp_viewmodel>
	{
	};
}
