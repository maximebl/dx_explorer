#pragma once

#include "not_zero.g.h"

namespace winrt::transformations::implementation
{
    struct not_zero : not_zeroT<not_zero>
    {
        not_zero() = default;
		Windows::Foundation::IInspectable Convert(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language);
        Windows::Foundation::IInspectable ConvertBack(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language);
    };
}

namespace winrt::transformations::factory_implementation
{
    struct not_zero : not_zeroT<not_zero, implementation::not_zero>
    {
    };
}
