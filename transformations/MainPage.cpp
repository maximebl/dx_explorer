#include "pch.h"
#include "MainPage.h"
#include "MainPage.g.cpp"

using namespace winrt;
using namespace winrt::Windows::UI::Xaml;

namespace winrt::transformations::implementation
{
	MainPage::MainPage()
	{
		InitializeComponent();
	}

	void MainPage::navigation_view_invoked(Windows::Foundation::IInspectable const &, Windows::UI::Xaml::Controls::NavigationViewItemInvokedEventArgs const & args)
	{
		hstring current_page_typename = content_frame().CurrentSourcePageType().Name;
		hstring target_page_typename = unbox_value_or<hstring>(args.InvokedItemContainer().Tag(), hstring{ L"empty" });

		if (current_page_typename != target_page_typename)
		{
			Windows::UI::Xaml::Interop::TypeName page_type_name;
			page_type_name.Name = unbox_value<hstring>(args.InvokedItemContainer().Tag());
			page_type_name.Kind = Windows::UI::Xaml::Interop::TypeKind::Primitive;
			content_frame().Navigate(page_type_name, nullptr);
		}
	}
}
