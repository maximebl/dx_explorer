#pragma once

#include "MainPage.g.h"

namespace winrt::transformations::implementation
{
    struct MainPage : MainPageT<MainPage>
    {
        MainPage();
		void navigation_view_invoked(Windows::Foundation::IInspectable const & /* sender */, Windows::UI::Xaml::Controls::NavigationViewItemInvokedEventArgs const & args);
    };
}

namespace winrt::transformations::factory_implementation
{
    struct MainPage : MainPageT<MainPage, implementation::MainPage>
    {
    };
}
