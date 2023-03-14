#pragma once
#include "VK.h"
#include "VKFence.h"
#include "Reference.h"

namespace Lumos
{
	enum class CommandBufferState :uint8_t
	{
		Idle,
		Recording,
		Ended,
		Submitted
	};

	class VKCommandBuffer
	{
	public:
		VKCommandBuffer();
		~VKCommandBuffer();

		void Reset();
		bool Wait();

		bool Init(bool primary, VkCommandPool commandPool);
		VkCommandBuffer GetHandle() const { return m_CommandBuffer; };
		//TODO
		SharedPtr<VKFence> GetFence() { return m_Fence; };

		CommandBufferState GetState() const { return m_State; }
		VkSemaphore GetSemaphore() { return m_Semaphore; };

	private:
		VkCommandBuffer m_CommandBuffer;
		VkCommandPool m_CommandPool;

		bool m_Primary;
		CommandBufferState m_State;

		SharedPtr<VKFence> m_Fence;

		VkSemaphore m_Semaphore;
	};
}