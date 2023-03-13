#include "VKDevice.h"

namespace Lumos
{
	VKDevice::VKDevice()
	{

	}

	VKDevice::~VKDevice()
	{
		vkDestroyDevice(m_Device, VK_NULL_HANDLE);
	}

	bool VKDevice::Init()
	{
		m_PhysicalDevice = std::make_shared<VKPhysicalDevice>();

		VkPhysicalDeviceFeatures supportedFeatures;
		memset(&supportedFeatures, 0, sizeof(VkPhysicalDeviceFeatures));
		memset(&m_EnabledFeatures, 0, sizeof(VkPhysicalDeviceFeatures));

		vkGetPhysicalDeviceFeatures(m_PhysicalDevice->GetHandle(), &supportedFeatures);

		if(supportedFeatures.wideLines)
		{
			m_EnabledFeatures.wideLines = true;
		}

		if (supportedFeatures.samplerAnisotropy)
		{
			m_EnabledFeatures.samplerAnisotropy = true;
		}

		std::vector<const char*> deviceExtensions = {
				VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		if (m_PhysicalDevice->IsExtensionSupported(VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
		{
			deviceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
			m_EnableDebugMarkers = true;
		}

		VkPhysicalDeviceDescriptorIndexingFeaturesEXT indexingFeatures = {};
		indexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;
		indexingFeatures.runtimeDescriptorArray = VK_TRUE;
		indexingFeatures.descriptorBindingPartiallyBound = VK_TRUE;

		// Device
		VkDeviceCreateInfo deviceCreateInfo = {};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(m_PhysicalDevice->m_QueueCreateInfos.size());
		deviceCreateInfo.pQueueCreateInfos = m_PhysicalDevice->m_QueueCreateInfos.data();
		deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
		deviceCreateInfo.pEnabledFeatures = &m_EnabledFeatures;
		deviceCreateInfo.enabledLayerCount = 0;
		deviceCreateInfo.pNext = (void*)&indexingFeatures;

		auto result = vkCreateDevice(m_PhysicalDevice->GetHandle(), &deviceCreateInfo, VK_NULL_HANDLE, &m_Device);
		if (result != VK_SUCCESS)
		{
			return false;
		}

		vkGetDeviceQueue(m_Device, m_PhysicalDevice->m_QueueFamilyIndices.Graphics, 0, &m_GraphicsQueue);
		vkGetDeviceQueue(m_Device, m_PhysicalDevice->m_QueueFamilyIndices.Graphics, 0, &m_PresentQueue);
		vkGetDeviceQueue(m_Device, m_PhysicalDevice->m_QueueFamilyIndices.Compute, 0, &m_ComputeQueue);

		return VK_SUCCESS;
	}
}