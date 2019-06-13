#pragma once

#include "fold_panel.g.h"

namespace winrt::transformations::implementation
{
	struct fold_panel : fold_panelT<fold_panel>
	{
		fold_panel();

		static Windows::UI::Xaml::DependencyProperty LabelProperty();
		hstring Label();
		void Label(hstring value);

		//static Windows::UI::Xaml::DependencyProperty ChildrenProperty();
		//Windows::UI::Xaml::Controls::UIElementCollection Children();
		//void Children(Windows::UI::Xaml::Controls::UIElementCollection value);

		static void OnLabelChanged(Windows::UI::Xaml::DependencyObject const&, Windows::UI::Xaml::DependencyPropertyChangedEventArgs const&);

	private:
		static Windows::UI::Xaml::DependencyProperty m_labelProperty;
		//static Windows::UI::Xaml::DependencyProperty m_ChildrenProperty;
	};
}

namespace winrt::transformations::factory_implementation
{
	struct fold_panel : fold_panelT<fold_panel, implementation::fold_panel>
	{
	};
}
