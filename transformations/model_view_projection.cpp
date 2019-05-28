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

		m_vm = winrt::make<transformations::implementation::mvp_viewmodel>();
		m_vm.Title(L"sup");
		m_vm.current_swapchain_panel(swapchain_panel());

		DataContext(m_vm);
    }

	transformations::mvp_viewmodel model_view_projection::vm()
	{
		return m_vm;
	}
}
