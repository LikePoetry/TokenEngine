#pragma once
#include "VK.h"

namespace Lumos
{
	class VKCommandBuffer
	{
	public:
		VKCommandBuffer();
		~VKCommandBuffer();

		bool Init(bool primary, VkCommandPool commandPool);
		VkCommandBuffer GetHandle() const { return m_CommandBuffer; };
	private:
		VkCommandBuffer m_CommandBuffer;
		VkCommandPool m_CommandPool;

		bool m_Primary;

		VkSemaphore m_Semaphore;
	};
}