#include "VKCommandPool.h"

#include "VKDevice.h"

namespace Lumos
{
	VKCommandPool::VKCommandPool(int queueIndex,VkCommandPoolCreateFlags flags)
	{
		VkCommandPoolCreateInfo cmdPoolCI{};
		cmdPoolCI.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		cmdPoolCI.flags = flags;
		cmdPoolCI.queueFamilyIndex = queueIndex;

		vkCreateCommandPool(VKDevice::Get().GetHandle(), &cmdPoolCI, nullptr, &m_Handle);
	}

	VKCommandPool::~VKCommandPool()
	{
		vkDestroyCommandPool(VKDevice::Get().GetHandle(), m_Handle, nullptr);
	}

	void VKCommandPool::Reset()
	{
		vkResetCommandPool(VKDevice::Get().GetDevice(), m_Handle, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);
	}
}