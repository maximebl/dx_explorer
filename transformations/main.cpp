#include "pch.h"
#include "App.h"

int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
	winrt::Windows::UI::Xaml::Application::Start(
		[](auto &&)
	{
		winrt::make<winrt::transformations::implementation::App>();
	});
	return 0;
}