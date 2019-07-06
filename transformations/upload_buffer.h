#pragma once
template <typename T>
class upload_buffer
{
public:
	enum class buffer_type
	{
		constant_buffer = 0
	};

	winrt::com_ptr<ID3D12Resource> upload_buffer_resource;
	size_t element_count = 0;
	size_t element_size = 0;

private:
	std::byte* m_mapped_data = nullptr;
	buffer_type m_buffer_type = buffer_type::constant_buffer;

public:
	~upload_buffer()
	{
	}

	upload_buffer(ID3D12Device* device, size_t element_count, buffer_type buffer_type)
		: element_count(element_count), m_buffer_type(buffer_type)
	{
		element_size = sizeof(T);
		if (buffer_type == buffer_type::constant_buffer)
		{
			// Constant data reads must be a multiple of 256 bytes from the beginning of the heap. (aka 256 bytes aligned)
			// https://docs.microsoft.com/en-ca/windows/desktop/direct3d12/upload-and-readback-of-texture-data
			element_size = (element_size + 255) & ~255;
		}

		winrt::check_hresult(device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(element_size * element_count),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			winrt::guid_of<ID3D12Resource>(),
			upload_buffer_resource.put_void()));

		winrt::check_hresult(
			// Map: Gets a CPU pointer to the specified subresource (0) in the upload_buffer Resource (upload_buffer).
			// Also invalidate the CPU cache (if needed) so that CPU reads to this address reflect modifications made by the GPU.
			// m_mapped_data is the pointer to the pointer to the resource data.
			upload_buffer_resource->Map(0, nullptr, reinterpret_cast<void**>(&m_mapped_data)));
	}

	void copy_data(int element_index, const T & data)
	{
		std::memcpy(&m_mapped_data[element_index * element_size], &data, sizeof(T));
	}
};

