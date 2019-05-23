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
				if (!app.initialize(m_current_swapchain_panel))
				{
					co_await ui_helpers::show_error_dialog(L"Failed to verify DirectX Math library support.", L"CPU not supported");
					co_return;
				}
				app.load_content();
				app.run();
			});
	}

	hstring mvp_viewmodel::Title()
	{
		return m_title;
	}

	void mvp_viewmodel::Title(hstring value)
	{
		update_value(L"Title", m_title, value);
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
}
