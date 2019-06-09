#pragma once

#include "xaml_helpers.g.h"

namespace winrt::transformations::implementation
{
    struct xaml_helpers : xaml_helpersT<xaml_helpers>
    {
        xaml_helpers() = default;
		static double not_zero(float input_value);
		static float not_zero_bindback(double input_value);
    };
}

namespace winrt::transformations::factory_implementation
{
    struct xaml_helpers : xaml_helpersT<xaml_helpers, implementation::xaml_helpers>
    {
    };
}
