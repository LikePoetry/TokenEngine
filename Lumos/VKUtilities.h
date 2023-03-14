#pragma once
#include "VK.h"
#include <vector>

#define VK_CHECK_RESULT(f)	\
{							\
	VkResult res = (f);		\
	if (res!=VK_SUCCESS)	\
	{						\
							\
	}						\
}

namespace Lumos
{
	namespace VKUtilities
	{

		void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels = 1, uint32_t layerCount = 1, VkCommandBuffer commandBuffer = nullptr);

		bool IsPresentModeSupported(const std::vector<VkPresentModeKHR>& supportedModes, VkPresentModeKHR presentMode);
		VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& supportedModes, bool vsync);
	}
}