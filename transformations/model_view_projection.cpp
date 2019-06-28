﻿#include "pch.h"
#include "model_view_projection.h"
#include "model_view_projection.g.cpp"

using namespace winrt;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::Foundation;

namespace winrt::transformations::implementation
{
	model_view_projection::model_view_projection()
	{
		DataContext(m_vm);
		InitializeComponent();
		m_vm.current_swapchain_panel(swapchain_panel());
	}

	transformations::mvp_viewmodel model_view_projection::vm()
	{
		return m_vm;
	}

	IAsyncAction model_view_projection::page_loaded(IInspectable const & sender, RoutedEventArgs const & args)
	{
		//winrt::Windows::UI::Xaml::ElementFactoryGetArgs argz;
		//auto elems = mesh_list().ItemTemplate().GetElement(argz);
		//auto a = elems.as<winrt::Windows::UI::Xaml::FrameworkElement>();
		//auto name = a.Name();
		//a.DataContext(m_vm);
		//this->Bindings->Update();

		m_vm.initialize_app().Execute(nullptr);
		co_return;
	}

	IAsyncAction model_view_projection::page_size_changed(IInspectable const & sender, RoutedEventArgs const & args)
	{
		auto new_width = swapchain_panel().ActualWidth();
		auto new_height = swapchain_panel().ActualHeight();
		m_vm.viewport_width(new_width);
		m_vm.viewport_height(new_height);
		m_vm.scissor_rect_width(new_width);
		m_vm.scissor_rect_height(new_height);
		co_return;
	}

	IAsyncAction model_view_projection::up_direction_checked(IInspectable const & sender, RoutedEventArgs const & args)
	{
		auto rb = sender.as<Windows::UI::Xaml::Controls::RadioButton>();
		hstring tag = unbox_value<hstring>(rb.Tag());
		transformations::vector_selection current_selection;

		if (tag == L"X")current_selection = transformations::vector_selection::x;
		if (tag == L"Y")current_selection = transformations::vector_selection::y;
		if (tag == L"Z")current_selection = transformations::vector_selection::z;
		if (tag == L"W")current_selection = transformations::vector_selection::w;

		m_vm.up_direction(current_selection);
		co_return;
	}

	IAsyncAction model_view_projection::focus_point_checked(IInspectable const & sender, RoutedEventArgs const & args)
	{
		auto rb = sender.as<Windows::UI::Xaml::Controls::RadioButton>();
		hstring tag = unbox_value<hstring>(rb.Tag());
		transformations::vector_selection current_selection;

		if (tag == L"X")current_selection = transformations::vector_selection::x;
		if (tag == L"Y")current_selection = transformations::vector_selection::y;
		if (tag == L"Z")current_selection = transformations::vector_selection::z;
		if (tag == L"W")current_selection = transformations::vector_selection::w;

		m_vm.focus_point(current_selection);
		co_return;
	}
}
