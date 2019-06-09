#include "pch.h"
#include "enum_to_bool.h"
#if __has_include("enum_to_bool.g.cpp")
#include "enum_to_bool.g.cpp"
#endif

using namespace winrt;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI::Xaml::Interop;

namespace winrt::transformations::implementation
{
	Windows::Foundation::IInspectable enum_to_bool::Convert(IInspectable const& value, TypeName const& targetType, IInspectable const& parameter, hstring const& language)
	{
		auto bbb = unbox_value<transformations::vector_selection>(parameter);
		//auto abc = unbox_value(parameter);
		return value;
	}

	Windows::Foundation::IInspectable enum_to_bool::ConvertBack(IInspectable const& value, TypeName const& targetType, IInspectable const& parameter, hstring const& language)
	{
		return value;
	}
}
