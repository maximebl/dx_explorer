#include "pch.h"
#include "ui_helpers.h"

using namespace winrt;
using namespace winrt::Windows::Foundation;

ui_helpers::ui_helpers()
{
}


ui_helpers::~ui_helpers()
{
}

IAsyncAction ui_helpers::show_error_dialog(hstring error_message, hstring title)
{
	auto dialog = winrt::Windows::UI::Xaml::Controls::ContentDialog();
	dialog.Title(box_value(title));
	dialog.Content(box_value(error_message));
	dialog.CloseButtonText(L"Ok");
	co_await dialog.ShowAsync();
	co_return;
}
