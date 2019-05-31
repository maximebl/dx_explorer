#pragma once
class upload_buffer
{
public:
	upload_buffer(ID3D12Device* device);
	~upload_buffer();

private:
	winrt::com_ptr<ID3D12Resource> m_upload_buffer;
};

