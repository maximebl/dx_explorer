#include "pch.h"
#include "model_view_projection.h"
#include "model_view_projection.g.cpp"

using namespace winrt;
using namespace winrt::Windows::UI::Xaml;

namespace winrt::transformations::implementation
{
	model_view_projection::model_view_projection()
	{
		InitializeComponent();

		DataContext(m_vm);
		m_vm.current_swapchain_panel(swapchain_panel());

		m_app.initialize(m_vm);
		m_app.load_content();
		m_app.run();
	}

	transformations::mvp_viewmodel model_view_projection::vm()
	{
		return m_vm;
	}
}
