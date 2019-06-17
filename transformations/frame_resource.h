#pragma once
#include "render_item.h"

class frame_resource
{
public:
	frame_resource(winrt::com_ptr<ID3D12Device> device);
	~frame_resource();

	uint64_t fence_value = 0;
	winrt::com_ptr<ID3D12CommandAllocator> cmd_allocator;
};

