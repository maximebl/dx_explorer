#include "pch.h"
#include "mesh_vm.h"
#include "mesh_vm.g.cpp"

using namespace winrt;
using namespace winrt::Windows::UI::Xaml::Input;
using namespace winrt::Windows::Foundation;

namespace winrt::transformations::implementation
{
	mesh_vm::mesh_vm()
	{
		m_cmd_delete_mesh.ExecuteRequested(
			[this](XamlUICommand sender, ExecuteRequestedEventArgs args) -> IAsyncAction {
			delete_mesh();
			co_return;
		});
	}

	winrt::Windows::Foundation::IAsyncAction mesh_vm::keydown(Windows::Foundation::IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args)
	{
		return winrt::Windows::Foundation::IAsyncAction();
	}

	Windows::UI::Xaml::Input::StandardUICommand mesh_vm::cmd_delete_mesh()
	{
		return m_cmd_delete_mesh;
	}

	bool mesh_vm::is_selected()
	{
		return m_is_selected;
	}

	void mesh_vm::is_selected(bool value)
	{
		update_value(L"is_selected", m_is_selected, value);
	}

	hstring mesh_vm::mesh_type()
	{
		return m_mesh_type;
	}

	void mesh_vm::mesh_type(hstring value)
	{
		update_value(L"mesh_type", m_mesh_type, value);
	}

	uint32_t mesh_vm::index()
	{
		return m_index;
	}

	void mesh_vm::index(uint32_t value)
	{
		update_value(L"index", m_index, value);
	}

	transformations::float3 mesh_vm::scale()
	{
		return m_scale;
	}

	void mesh_vm::scale(transformations::float3 value)
	{
		update_value(L"scale", m_scale, value);
	}

	transformations::float3 mesh_vm::rotation_axis()
	{
		return m_rotation_axis;
	}

	void mesh_vm::rotation_axis(transformations::float3 value)
	{
		update_value(L"rotation_axis", m_rotation_axis, value);
	}

	transformations::float3 mesh_vm::translation()
	{
		return m_translation;
	}

	void mesh_vm::translation(transformations::float3 value)
	{
		update_value(L"translation", m_translation, value);
	}

	float mesh_vm::angle()
	{
		return m_angle;
	}

	void mesh_vm::angle(float value)
	{
		update_value(L"angle", m_angle, value);
	}

	void mesh_vm::delete_mesh()
	{
	}
}