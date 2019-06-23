#include "pch.h"
#include "scalar_slider.h"
#if __has_include("scalar_slider.g.cpp")
#include "scalar_slider.g.cpp"
#endif

using namespace winrt;
using namespace winrt::Windows::UI::Xaml;

namespace winrt::transformations::implementation
{
	scalar_slider::scalar_slider()
	{
		InitializeComponent();
	}

	winrt::Windows::Foundation::IAsyncAction scalar_slider::manip_starting(Windows::Foundation::IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args)
	{
		is_manipulating(true);
		co_return;
	}

	winrt::Windows::Foundation::IAsyncAction scalar_slider::manip_completed(Windows::Foundation::IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args)
	{
		is_manipulating(false);
		co_return;
	}

	winrt::event_token scalar_slider::PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
	{
		return m_property_changed.add(handler);
	}

	void scalar_slider::PropertyChanged(winrt::event_token const& token) noexcept
	{
		m_property_changed.remove(token);
	}

	Windows::UI::Xaml::DependencyProperty scalar_slider::m_label_property = Windows::UI::Xaml::DependencyProperty::Register(
		L"label",
		winrt::xaml_typename<winrt::hstring>(),
		winrt::xaml_typename<winrt::transformations::scalar_slider>(),
		Windows::UI::Xaml::PropertyMetadata{ nullptr }
	);

	Windows::UI::Xaml::DependencyProperty scalar_slider::m_scalar_value_property = Windows::UI::Xaml::DependencyProperty::Register(
		L"scalar_value",
		winrt::xaml_typename<float>(),
		winrt::xaml_typename<winrt::transformations::scalar_slider>(),
		Windows::UI::Xaml::PropertyMetadata{ nullptr }
	);

	Windows::UI::Xaml::DependencyProperty scalar_slider::m_scalar_min_property = Windows::UI::Xaml::DependencyProperty::Register(
		L"scalar_min",
		winrt::xaml_typename<double>(),
		winrt::xaml_typename<winrt::transformations::scalar_slider>(),
		Windows::UI::Xaml::PropertyMetadata{ nullptr }
	);

	Windows::UI::Xaml::DependencyProperty scalar_slider::m_scalar_max_property = Windows::UI::Xaml::DependencyProperty::Register(
		L"scalar_max",
		winrt::xaml_typename<double>(),
		winrt::xaml_typename<winrt::transformations::scalar_slider>(),
		Windows::UI::Xaml::PropertyMetadata{ nullptr }
	);

	Windows::UI::Xaml::DependencyProperty scalar_slider::m_step_frequency_property = Windows::UI::Xaml::DependencyProperty::Register(
		L"step_frequency",
		winrt::xaml_typename<double>(),
		winrt::xaml_typename<winrt::transformations::scalar_slider>(),
		Windows::UI::Xaml::PropertyMetadata{ nullptr }
	);

	Windows::UI::Xaml::DependencyProperty scalar_slider::m_is_manipulating_property = Windows::UI::Xaml::DependencyProperty::Register(
		L"is_manipulating",
		winrt::xaml_typename<bool>(),
		winrt::xaml_typename<winrt::transformations::scalar_slider>(),
		Windows::UI::Xaml::PropertyMetadata{ nullptr }
	);

	Windows::UI::Xaml::DependencyProperty scalar_slider::labelProperty()
	{
		return m_label_property;
	}

	Windows::UI::Xaml::DependencyProperty scalar_slider::scalar_valueProperty()
	{
		return m_scalar_value_property;
	}

	Windows::UI::Xaml::DependencyProperty scalar_slider::scalar_minProperty()
	{
		return m_scalar_min_property;
	}

	Windows::UI::Xaml::DependencyProperty scalar_slider::scalar_maxProperty()
	{
		return m_scalar_max_property;
	}

	Windows::UI::Xaml::DependencyProperty scalar_slider::step_frequencyProperty()
	{
		return m_step_frequency_property;
	}

	Windows::UI::Xaml::DependencyProperty scalar_slider::is_manipulatingProperty()
	{
		return m_is_manipulating_property;
	}

	hstring scalar_slider::label()
	{
		return m_label;
	}

	void scalar_slider::label(hstring value)
	{
		update_value(L"label", m_label, value);
	}

	float scalar_slider::scalar_value()
	{
		return winrt::unbox_value<float>(GetValue(m_scalar_value_property));
	}

	void scalar_slider::scalar_value(float value)
	{
		SetValue(m_scalar_value_property, winrt::box_value(value));
		update_value(L"scalar_value", m_scalar_value, value);
	}

	double scalar_slider::scalar_min()
	{
		return winrt::unbox_value<double>(GetValue(m_scalar_min_property));
	}

	void scalar_slider::scalar_min(double value)
	{
		SetValue(m_scalar_min_property, winrt::box_value(value));
		update_value(L"scalar_min", m_scalar_min, value);
	}

	double scalar_slider::scalar_max()
	{
		return winrt::unbox_value<double>(GetValue(m_scalar_max_property));
	}

	void scalar_slider::scalar_max(double value)
	{
		SetValue(m_scalar_max_property, winrt::box_value(value));
		update_value(L"scalar_max", m_scalar_max, value);
	}

	double scalar_slider::step_frequency()
	{
		return winrt::unbox_value<double>(GetValue(m_step_frequency_property));
	}

	void scalar_slider::step_frequency(double value)
	{
		SetValue(m_step_frequency_property, winrt::box_value(value));
		update_value(L"step_frequency", m_step_frequency, value);
	}

	bool scalar_slider::is_manipulating()
	{
		return winrt::unbox_value<bool>(GetValue(m_is_manipulating_property));
	}

	void scalar_slider::is_manipulating(bool value)
	{
		SetValue(m_is_manipulating_property, winrt::box_value(value));
		update_value(L"is_manipulating", m_is_manipulating, value);
	}
}
