#pragma once

#include "model_view_projection.g.h"
#include "mvp_viewmodel.h"

namespace winrt::transformations::implementation
{
	struct model_view_projection : model_view_projectionT<model_view_projection>
	{
		model_view_projection();

		transformations::mvp_viewmodel vm();

	private:
		transformations::mvp_viewmodel m_vm = nullptr;
	};
}

namespace winrt::transformations::factory_implementation
{
	struct model_view_projection : model_view_projectionT<model_view_projection, implementation::model_view_projection>
	{
	};
}
