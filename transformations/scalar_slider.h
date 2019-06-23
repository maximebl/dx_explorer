#pragma once

#include "winrt/Windows.UI.Xaml.h"
#include "winrt/Windows.UI.Xaml.Markup.h"
#include "winrt/Windows.UI.Xaml.Interop.h"
#include "winrt/Windows.UI.Xaml.Controls.Primitives.h"
#include "scalar_slider.g.h"

namespace winrt::transformations::implementation
{
	struct scalar_slider : scalar_sliderT<scalar_slider>
	{
		scalar_slider();
		winrt::Windows::Foundation::IAsyncAction manip_starting(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
		winrt::Windows::Foundation::IAsyncAction manip_completed(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);

		static Windows::UI::Xaml::DependencyProperty labelProperty();
		static Windows::UI::Xaml::DependencyProperty scalar_valueProperty();
		static Windows::UI::Xaml::DependencyProperty scalar_minProperty();
		static Windows::UI::Xaml::DependencyProperty scalar_maxProperty();
		static Windows::UI::Xaml::DependencyProperty step_frequencyProperty();
		static Windows::UI::Xaml::DependencyProperty is_manipulatingProperty();

		hstring label();
		void label(hstring value);

		float scalar_value();
		void scalar_value(float value);

		double scalar_min();
		void scalar_min(double value);

		double scalar_max();
		void scalar_max(double value);

		double step_frequency();
		void step_frequency(double value);

		bool is_manipulating();
		void is_manipulating(bool value);

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

		static Windows::UI::Xaml::DependencyProperty m_label_property;
		static Windows::UI::Xaml::DependencyProperty m_scalar_value_property;
		static Windows::UI::Xaml::DependencyProperty m_scalar_min_property;
		static Windows::UI::Xaml::DependencyProperty m_scalar_max_property;
		static Windows::UI::Xaml::DependencyProperty m_step_frequency_property;
		static Windows::UI::Xaml::DependencyProperty m_is_manipulating_property;

		hstring m_label = L"placeholder label";
		float m_scalar_value = 0.0f;
		double m_scalar_min = 0.0;
		double m_scalar_max = 10.0;
		double m_step_frequency = 0.1;
		bool m_is_manipulating = false;
	};
}

namespace winrt::transformations::factory_implementation
{
	struct scalar_slider : scalar_sliderT<scalar_slider, implementation::scalar_slider>
	{
	};
}
