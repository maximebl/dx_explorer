#include "pch.h"
#include "cmd_queue.h"

using namespace winrt;

cmd_queue::cmd_queue(com_ptr<ID3D12Device5> device) : m_device(device)
{
	D3D12_COMMAND_QUEUE_DESC cmd_queue_desc = {};
	cmd_queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAGS::D3D12_COMMAND_QUEUE_FLAG_NONE;
	cmd_queue_desc.NodeMask = 0;
	cmd_queue_desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY::D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	cmd_queue_desc.Type = D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT;
	device->CreateCommandQueue(&cmd_queue_desc, guid_of<ID3D12CommandQueue>(), m_cmd_queue.put_void());

	device->CreateFence(0, D3D12_FENCE_FLAGS::D3D12_FENCE_FLAG_NONE, guid_of<ID3D12Fence1>(), m_fence.put_void());
	m_fence_event = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
}

cmd_queue::~cmd_queue()
{
	CloseHandle(m_fence_event);
}

void cmd_queue::flush_cmd_queue()
{
	wait_for_fence_value(signal());
}

void cmd_queue::execute_cmd_list(com_ptr<ID3D12GraphicsCommandList4> cmd_list)
{
	cmd_list->Close();
	std::array<ID3D12CommandList*, 1> cmd_lists = { cmd_list.get() };
	m_cmd_queue->ExecuteCommandLists((UINT)cmd_lists.size(), &cmd_lists[0]);
}

void cmd_queue::wait_for_fence_value(uint64_t value)
{
	if (!is_fence_complete(value))
	{
		m_fence->SetEventOnCompletion(value, m_fence_event);
		WaitForSingleObject(m_fence_event, INFINITE);
	}
}

bool cmd_queue::is_fence_complete(uint64_t value)
{
	return m_fence->GetCompletedValue() >= value;
}

uint64_t cmd_queue::signal()
{
	uint64_t fence_value = ++m_fence_value;
	m_cmd_queue->Signal(m_fence.get(), fence_value);
	return fence_value;
}

winrt::com_ptr<ID3D12CommandQueue> cmd_queue::get_cmd_queue()
{
	return m_cmd_queue;
}
