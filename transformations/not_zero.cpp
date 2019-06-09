#include "pch.h"
#include "not_zero.h"
#if __has_include("not_zero.g.cpp")
#include "not_zero.g.cpp"
#endif

#include "math_helpers.h"

using namespace winrt;
using namespace winrt::Windows::UI::Xaml;

namespace winrt::transformations::implementation
{
	Windows::Foundation::IInspectable not_zero::Convert(Windows::Foundation::IInspectable const & value, Windows::UI::Xaml::Interop::TypeName const & targetType, Windows::Foundation::IInspectable const & parameter, hstring const & language)
	{
		float new_value = unbox_value<float>(value);
		bool is_zero = math_helpers::is_nearly_zero(new_value);

		if(!is_zero)
		{
			return value;
		}
		else
		{
			new_value = 0.1;
			return box_value(new_value);
		}
	}

	Windows::Foundation::IInspectable not_zero::ConvertBack(Windows::Foundation::IInspectable const & value, Windows::UI::Xaml::Interop::TypeName const & targetType, Windows::Foundation::IInspectable const & parameter, hstring const & language)
	{
		float new_value = unbox_value<float>(value);
		bool is_zero = math_helpers::is_nearly_zero(new_value);

		if(!is_zero)
		{
			return value;
		}
		else
		{
			new_value = 0.1;
			return box_value(new_value);
		}
	}
}
