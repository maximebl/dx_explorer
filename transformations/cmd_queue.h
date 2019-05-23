#pragma once
//#include <queue>

class cmd_queue
{
public:
	cmd_queue(winrt::com_ptr<ID3D12Device5> device);
	~cmd_queue();

	void flush_cmd_queue();
	void execute_cmd_list(winrt::com_ptr<ID3D12GraphicsCommandList4> cmd_list);
	void wait_for_fence_value(uint64_t value);
	bool is_fence_complete(uint64_t value);
	uint64_t signal();
	winrt::com_ptr<ID3D12CommandQueue> get_cmd_queue();
	uint64_t m_fence_value = 0;

private:
	winrt::com_ptr<ID3D12Device5> m_device;
	winrt::com_ptr<ID3D12CommandQueue> m_cmd_queue;
	winrt::com_ptr<ID3D12Fence1> m_fence;

	HANDLE m_fence_event;
};

