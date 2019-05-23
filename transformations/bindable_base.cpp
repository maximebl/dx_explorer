#include "pch.h"
#include "bindable_base.h"
#include "bindable_base.g.cpp"

namespace winrt::transformations::implementation
{
	void bindable_base::RaisePropertyChanged(hstring const& propertyName)
	{
		m_property_changed(*this, Windows::UI::Xaml::Data::PropertyChangedEventArgs(propertyName));
	}

	winrt::event_token bindable_base::PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
	{
		return m_property_changed.add(handler);
	}

	void bindable_base::PropertyChanged(winrt::event_token const& token) noexcept
	{
		m_property_changed.remove(token);
	}
}
