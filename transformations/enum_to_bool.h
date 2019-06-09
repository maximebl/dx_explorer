#pragma once

#include "enum_to_bool.g.h"

namespace winrt::transformations::implementation
{
    struct enum_to_bool : enum_to_boolT<enum_to_bool>
    {
        enum_to_bool() = default;
		Windows::Foundation::IInspectable Convert(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language);
        Windows::Foundation::IInspectable ConvertBack(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language);
    };
}

namespace winrt::transformations::factory_implementation
{
    struct enum_to_bool : enum_to_boolT<enum_to_bool, implementation::enum_to_bool>
    {
    };
}
