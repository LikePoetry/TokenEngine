#pragma once
#include "VK.h"
#include "VKFence.h"
#include "Reference.h"

namespace Lumos
{
	class VKCommandBuffer
	{
	public:
		VKCommandBuffer();
		~VKCommandBuffer();

		bool Init(bool primary, VkCommandPool commandPool);
		VkCommandBuffer GetHandle() const { return m_CommandBuffer; };
		//TODO
		SharedPtr<VKFence> GetFence() { return m_Fence; };
		VkSemaphore GetSemaphore() { return m_Semaphore; };

	private:
		VkCommandBuffer m_CommandBuffer;
		VkCommandPool m_CommandPool;

		bool m_Primary;
		SharedPtr<VKFence> m_Fence;

		VkSemaphore m_Semaphore;
	};
}