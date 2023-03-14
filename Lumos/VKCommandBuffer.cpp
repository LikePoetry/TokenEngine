#include "VKCommandBuffer.h"
#include "VKInitialisers.h"
#include "VKDevice.h"
#include "VKUtilities.h"

namespace Lumos
{
	VKCommandBuffer::VKCommandBuffer()
		:m_CommandBuffer(nullptr)
		, m_CommandPool(nullptr)
		, m_Semaphore(nullptr)
		, m_Primary(false)
		, m_State(CommandBufferState::Idle)
	{

	}

	VKCommandBuffer::~VKCommandBuffer()
	{

	}

	bool VKCommandBuffer::Init(bool primary, VkCommandPool commandPool)
	{
		m_Primary = primary;
		m_CommandPool = commandPool;

		VkCommandBufferAllocateInfo cmdBufferCreateInfo = VKInitialisers::CommandBufferAllocateInfo(
			m_CommandPool, primary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY, 1);

		VK_CHECK_RESULT(vkAllocateCommandBuffers(VKDevice::Get().GetDevice(), &cmdBufferCreateInfo, &m_CommandBuffer));

		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		semaphoreInfo.pNext = nullptr;

		VK_CHECK_RESULT(vkCreateSemaphore(VKDevice::Get().GetDevice(), &semaphoreInfo, nullptr, &m_Semaphore));

		//TODO Fence ³éÏó
		m_Fence = CreateSharedPtr<VKFence>(true);
		return true;
	}

	bool VKCommandBuffer::Wait()
	{
		_ASSERT(m_State == CommandBufferState::Submitted);

		m_Fence->WaitAndReset();
		m_State = CommandBufferState::Idle;

		return true;
	}

	void VKCommandBuffer::Reset()
	{
		VK_CHECK_RESULT(vkResetCommandBuffer(m_CommandBuffer, 0));
	}
}