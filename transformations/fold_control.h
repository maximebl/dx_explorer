#pragma once

#include "winrt/Windows.UI.Xaml.h"
#include "winrt/Windows.UI.Xaml.Markup.h"
#include "winrt/Windows.UI.Xaml.Interop.h"
#include "winrt/Windows.UI.Xaml.Controls.Primitives.h"
#include "fold_control.g.h"

namespace winrt::transformations::implementation
{
    struct fold_control : fold_controlT<fold_control>
    {
        fold_control();

		static Windows::UI::Xaml::DependencyProperty inner_contentProperty();
        Windows::UI::Xaml::FrameworkElement inner_content();
        void inner_content(Windows::UI::Xaml::FrameworkElement value);

		static Windows::UI::Xaml::DependencyProperty labelProperty();
		hstring label();
        void label(hstring value);

		winrt::Windows::Foundation::IAsyncAction visibility_toggle_click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);

	private:
		static Windows::UI::Xaml::DependencyProperty m_inner_content_property;
		static Windows::UI::Xaml::DependencyProperty m_label_property;
    };
}

namespace winrt::transformations::factory_implementation
{
    struct fold_control : fold_controlT<fold_control, implementation::fold_control>
    {
    };
}
