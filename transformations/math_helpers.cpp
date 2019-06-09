#include "pch.h"
#include "math_helpers.h"
#include <limits>

math_helpers::math_helpers()
{
}


math_helpers::~math_helpers()
{
}

bool math_helpers::is_nearly_zero(float value)
{
	float eps = std::numeric_limits<float>::epsilon();
	return std::abs(value) < eps;
}
