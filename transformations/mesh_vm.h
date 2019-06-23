#pragma once
#include "mesh_vm.g.h"
#include "bindable_base.h"
#include "float3.h"

namespace winrt::transformations::implementation
{
	struct mesh_vm : mesh_vmT<mesh_vm, transformations::implementation::bindable_base>
	{
		mesh_vm();
		winrt::Windows::Foundation::IAsyncAction keydown(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);

		Windows::UI::Xaml::Input::StandardUICommand cmd_delete_mesh();

		bool is_selected();
		void is_selected(bool value);

		hstring mesh_type();
		void mesh_type(hstring value);

		uint32_t index();
		void index(uint32_t value);

		transformations::float3 scale();
		void scale(transformations::float3 value);

		transformations::float3 rotation_axis();
		void rotation_axis(transformations::float3 value);

		transformations::float3 translation();
		void translation(transformations::float3 value);

		float angle();
		void angle(float value);

	private:
		void delete_mesh();
		bool m_is_selected = false;
		Windows::UI::Xaml::Input::StandardUICommand m_cmd_delete_mesh;
		hstring m_mesh_type = L"";
		uint32_t m_index = 0;
		transformations::float3 m_scale = winrt::make<transformations::implementation::float3>(1.0f, 1.0f, 1.0f);
		transformations::float3 m_rotation_axis = winrt::make<transformations::implementation::float3>(0.0f, 1.0f, 1.0f);
		transformations::float3 m_translation = winrt::make<transformations::implementation::float3>(0.0f, 0.0f, 0.0f);
		float m_angle = 0.0f;
	};
}

namespace winrt::transformations::factory_implementation
{
	struct mesh_vm : mesh_vmT<mesh_vm, implementation::mesh_vm>
	{
	};
}