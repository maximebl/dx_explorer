#include "pch.h"
#include "fold_control.h"
#if __has_include("fold_control.g.cpp")
#include "fold_control.g.cpp"
#endif

using namespace winrt;
using namespace winrt::Windows::UI::Xaml;

namespace winrt::transformations::implementation
{
	fold_control::fold_control()
	{
		InitializeComponent();
	}

	Windows::UI::Xaml::DependencyProperty fold_control::m_inner_content_property =
		Windows::UI::Xaml::DependencyProperty::Register(
			L"inner_content",
			winrt::xaml_typename<Windows::UI::Xaml::FrameworkElement>(),
			winrt::xaml_typename<transformations::fold_control>(),
			Windows::UI::Xaml::PropertyMetadata{ nullptr }
	);

	Windows::UI::Xaml::DependencyProperty fold_control::m_label_property =
		Windows::UI::Xaml::DependencyProperty::Register(
			L"label",
			winrt::xaml_typename<winrt::hstring>(),
			winrt::xaml_typename<transformations::fold_control>(),
			Windows::UI::Xaml::PropertyMetadata{nullptr}
	);


	Windows::UI::Xaml::DependencyProperty fold_control::inner_contentProperty()
	{
		return m_inner_content_property;
	}

	Windows::UI::Xaml::FrameworkElement fold_control::inner_content()
	{
		return winrt::unbox_value<Windows::UI::Xaml::FrameworkElement>(GetValue(m_inner_content_property));
	}

	void fold_control::inner_content(Windows::UI::Xaml::FrameworkElement value)
	{
		SetValue(m_inner_content_property, winrt::box_value(value));
	}

	Windows::UI::Xaml::DependencyProperty fold_control::labelProperty()
	{
		return m_label_property;
	}

	hstring fold_control::label()
	{
		return winrt::unbox_value<winrt::hstring>(GetValue(m_label_property));
	}

	void fold_control::label(hstring value)
	{
		SetValue(m_label_property, winrt::box_value(value));
	}

	winrt::Windows::Foundation::IAsyncAction fold_control::visibility_toggle_click(Windows::Foundation::IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args)
	{
		bool is_visible = false;
		if (content_control().Visibility() == Windows::UI::Xaml::Visibility::Visible)
		{
			is_visible = true;
		}

		is_visible ? VisualStateManager().GoToState(*this, L"hide_panel", false)
		           : VisualStateManager().GoToState(*this, L"show_panel", false);
		co_return;
	}
}
