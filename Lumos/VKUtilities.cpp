#include "VKUtilities.h"

namespace Lumos
{
    void VKUtilities::TransitionImageLayout(VkImage image,VkFormat format, 
        VkImageLayout oldImageLayout, VkImageLayout newImageLayout, 
        uint32_t mipLevels, uint32_t layerCount, VkCommandBuffer commandBuffer)
    {
        //bool singleTimeCommand = false;

        //if (!commandBuffer)
        //{
        //    commandBuffer = BeginSingleTimeCommands();
        //    singleTimeCommand = true;
        //}


    }

    bool VKUtilities::IsPresentModeSupported(const std::vector<VkPresentModeKHR>& supportedModes, VkPresentModeKHR presentMode)
    {
        for (const auto& mode : supportedModes)
        {
            if (mode == presentMode)
            {
                return true;
            }
        }
        return false;
    }

    VkPresentModeKHR VKUtilities::ChoosePresentMode(const std::vector<VkPresentModeKHR>& supportedModes, bool vsync)
    {
        VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
        if (!vsync)
        {
            if (IsPresentModeSupported(supportedModes, VK_PRESENT_MODE_IMMEDIATE_KHR))
            {
                presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
            }
            else
            {
                //("Failed to find supported presentation mode.");
            }
        }

        // Modes
        //  if(IsPresentModeSupported(supportedModes, VK_PRESENT_MODE_FIFO_RELAXED_KHR))
        //  {
        //      presentMode = VK_PRESENT_MODE_FIFO_RELAXED_KHR;
        //  }
        //  else if(IsPresentModeSupported(supportedModes, VK_PRESENT_MODE_MAILBOX_KHR))
        //  {
        //      presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
        //  }

        return presentMode;
    }
}