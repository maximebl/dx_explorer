#pragma once
class ui_helpers
{
public:
	ui_helpers();
	~ui_helpers();
	static winrt::Windows::Foundation::IAsyncAction show_error_dialog(winrt::hstring error_message, winrt::hstring title);
};

