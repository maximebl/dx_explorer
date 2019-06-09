#include "pch.h"
#include "xaml_helpers.h"
#if __has_include("xaml_helpers.g.cpp")
#include "xaml_helpers.g.cpp"
#endif

using namespace winrt;
using namespace winrt::Windows::UI::Xaml;

namespace winrt::transformations::implementation
{
	double xaml_helpers::not_zero(float input_value)
	{
		if ((input_value > 0 || input_value < 0) && input_value != -0 && input_value != 0)
		{
			return input_value;
		}
		else
		{
			return 0.5;
		}
	}

	float xaml_helpers::not_zero_bindback(double input_value)
	{
		if ((input_value > 0 || input_value < 0) && input_value != -0 && input_value != 0)
		{
			return input_value;
		}
		else
		{
			return 0.5;
		}
	}
}
