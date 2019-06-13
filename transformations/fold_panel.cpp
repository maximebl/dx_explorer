#include "pch.h"
#include "fold_panel.h"
#if __has_include("fold_panel.g.cpp")
#include "fold_panel.g.cpp"
#endif

using namespace winrt;
using namespace winrt::Windows::UI::Xaml;

namespace winrt::transformations::implementation
{
	fold_panel::fold_panel()
	{
		//DefaultStyleKey(winrt::box_value(L"transformations.fold_panel"));
	}

	Windows::UI::Xaml::DependencyProperty fold_panel::m_labelProperty =
		Windows::UI::Xaml::DependencyProperty::Register(
			L"Label",
			winrt::xaml_typename<winrt::hstring>(),
			winrt::xaml_typename<transformations::fold_panel>(),
			Windows::UI::Xaml::PropertyMetadata{ winrt::box_value(L"default label"), Windows::UI::Xaml::PropertyChangedCallback{ &fold_panel::OnLabelChanged } }
	);

	//Windows::UI::Xaml::DependencyProperty fold_panel::m_ChildrenProperty =
	//	Windows::UI::Xaml::DependencyProperty::Register(
	//		L"Children",
	//		winrt::xaml_typename<winrt::Windows::UI::Xaml::Controls::UIElementCollection>(),
	//		winrt::xaml_typename<transformations::fold_panel>(),
	//		Windows::UI::Xaml::PropertyMetadata{ nullptr }
	//);

	void fold_panel::OnLabelChanged(Windows::UI::Xaml::DependencyObject const& d, Windows::UI::Xaml::DependencyPropertyChangedEventArgs const& /* e */)
	{
		if (transformations::fold_panel theControl{ d.try_as<transformations::fold_panel>() })
		{
			// Call members of the projected type via theControl.

			transformations::implementation::fold_panel* ptr{ winrt::get_self<transformations::implementation::fold_panel>(theControl) };
			// Call members of the implementation type via ptr.
		}
	}

	Windows::UI::Xaml::DependencyProperty fold_panel::LabelProperty()
	{
		return m_labelProperty;
	}

	//Windows::UI::Xaml::DependencyProperty fold_panel::ChildrenProperty()
	//{
	//	return m_ChildrenProperty;
	//}

	//Windows::UI::Xaml::Controls::UIElementCollection fold_panel::Children()
	//{
	//	auto res = GetValue(m_ChildrenProperty);
	//	return unbox_value<winrt::Windows::UI::Xaml::Controls::UIElementCollection>(GetValue(m_ChildrenProperty));
	//}

	//void fold_panel::Children(Windows::UI::Xaml::Controls::UIElementCollection value)
	//{
	//	SetValue(m_ChildrenProperty, box_value(value));
	//}

	hstring fold_panel::Label()
	{
		return winrt::unbox_value<winrt::hstring>(GetValue(m_labelProperty));
	}

	void fold_panel::Label(hstring value)
	{
		SetValue(m_labelProperty, winrt::box_value(value));
	}
}
