#include "pch.h"
#include "float_to_double.h"
#if __has_include("float_to_double.g.cpp")
#include "float_to_double.g.cpp"
#endif

using namespace winrt;
using namespace winrt::Windows::UI::Xaml;

namespace winrt::transformations::implementation
{
	Windows::Foundation::IInspectable float_to_double::Convert(Windows::Foundation::IInspectable const & value, Windows::UI::Xaml::Interop::TypeName const & targetType, Windows::Foundation::IInspectable const & parameter, hstring const & language)
	{
		float new_value = unbox_value<float>(value);
		return box_value(static_cast<double>(new_value));
	}
	Windows::Foundation::IInspectable float_to_double::ConvertBack(Windows::Foundation::IInspectable const & value, Windows::UI::Xaml::Interop::TypeName const & targetType, Windows::Foundation::IInspectable const & parameter, hstring const & language)
	{
		float new_value = unbox_value<float>(value);
		return box_value(static_cast<double>(new_value));
	}
}
