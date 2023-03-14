#include "VKFence.h"
#include "VKDevice.h"
#include "VKUtilities.h"

namespace Lumos
{
	VKFence::VKFence(bool createSignaled)
	{
		m_Signaled = createSignaled;

		VkFenceCreateInfo fenceCreateInfo{};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.flags = createSignaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

		vkCreateFence(VKDevice::Get().GetDevice(), &fenceCreateInfo, nullptr, &m_Handle);
	}

	VKFence::~VKFence()
	{
		vkDestroyFence(VKDevice::Get().GetDevice(), m_Handle, nullptr);
	}

	bool VKFence::IsSignaled()
	{
		if (m_Signaled) {
			return true;
		}
		else
		{
			return CheckState();
		}
	}

	bool VKFence::CheckState()
	{
		const VkResult result = vkGetFenceStatus(VKDevice::Get().GetDevice(), m_Handle);
		if (result == VK_SUCCESS)
		{
			m_Signaled = true;
			return true;
		}

		return false;
	}

	bool VKFence::Wait()
	{
		_ASSERT(!m_Signaled);
		const VkResult result = vkWaitForFences(VKDevice::Get().GetDevice(), 1, &m_Handle, true, UINT32_MAX);

		VK_CHECK_RESULT(result);
		if (result == VK_SUCCESS)
		{
			m_Signaled = true;
			return false;
		}

		return true;
	}

	void VKFence::Reset()
	{
		if (m_Signaled)
			VK_CHECK_RESULT(vkResetFences(VKDevice::Get().GetDevice(), 1, &m_Handle));

		m_Signaled = false;
	}

	void VKFence::WaitAndReset()
	{
		if (!IsSignaled())
			Wait();

		Reset();
	}
}