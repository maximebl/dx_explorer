#pragma once

#include "float_to_double.g.h"

namespace winrt::transformations::implementation
{
    struct float_to_double : float_to_doubleT<float_to_double>
    {
        float_to_double() = default;
		Windows::Foundation::IInspectable Convert(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language);
        Windows::Foundation::IInspectable ConvertBack(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language);
    };
}

namespace winrt::transformations::factory_implementation
{
    struct float_to_double : float_to_doubleT<float_to_double, implementation::float_to_double>
    {
    };
}
