#pragma once
#include "bindable_base.g.h"

namespace winrt::transformations::implementation
{
	struct bindable_base : bindable_baseT<bindable_base>
	{
		bindable_base() = default;

		winrt::event_token PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
		void PropertyChanged(winrt::event_token const& token) noexcept;

		template <class T>
		void update_value(hstring const& property_name, T & var, T value)
		{
			if (var != value)
			{
				var = value;
				raise_property_changed(property_name);
			}
		}

	private:
		event<Windows::UI::Xaml::Data::PropertyChangedEventHandler> m_property_changed;

		void raise_property_changed(hstring const& property_name)
		{
			m_property_changed(*this, Windows::UI::Xaml::Data::PropertyChangedEventArgs(property_name));
		}
	};
}
namespace winrt::transformations::factory_implementation
{
	struct bindable_base : bindable_baseT<bindable_base, implementation::bindable_base>
	{
	};
}
