#include "VKPhysicalDevice.h"

#include "VKContext.h"


namespace Lumos
{
	VKPhysicalDevice::VKPhysicalDevice()
	{


	}

	VKPhysicalDevice::~VKPhysicalDevice()
	{

	}

	void VKPhysicalDevice::Init()
	{
		auto vkInstance = VKContext::GetVkInstance();
		vkEnumeratePhysicalDevices(vkInstance, &m_GPUCount, VK_NULL_HANDLE);

		if (m_GPUCount == 0)
		{
			// No GPUs found!
		}

		std::vector<VkPhysicalDevice> physicalDevices(m_GPUCount);

		vkEnumeratePhysicalDevices(vkInstance, &m_GPUCount, physicalDevices.data());
		// select at back!
		m_Handle = physicalDevices.back();

		// TODO от©╗сея║
		m_DeviceInfo = GetInfo(m_Handle);

		vkGetPhysicalDeviceProperties(m_Handle, &m_PhysicalDeviceProperties);
		vkGetPhysicalDeviceMemoryProperties(m_Handle, &m_MemoryProperties);


		uint32_t queueFamilyCount;
		vkGetPhysicalDeviceQueueFamilyProperties(m_Handle, &queueFamilyCount, nullptr);
		_ASSERTE(queueFamilyCount > 0, "");
		m_QueueFamilyProperties.resize(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(m_Handle, &queueFamilyCount, m_QueueFamilyProperties.data());

		uint32_t extCount = 0;
		vkEnumerateDeviceExtensionProperties(m_Handle, nullptr, &extCount, nullptr);
		if (extCount > 0)
		{
			std::vector<VkExtensionProperties> extensions(extCount);
			if (vkEnumerateDeviceExtensionProperties(m_Handle, nullptr, &extCount, &extensions.front()) == VK_SUCCESS)
			{
				for (const auto& ext : extensions)
				{
					m_SupportedExtensions.emplace(ext.extensionName);
				}
			}
		}

		static const float defaultQueuePriority(0.0f);

		int requestedQueueTypes = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT;
		m_QueueFamilyIndices = GetQueueFamilyIndices(requestedQueueTypes);

		// Graphics queue
		if (requestedQueueTypes & VK_QUEUE_GRAPHICS_BIT)
		{
			VkDeviceQueueCreateInfo queueInfo = {};
			queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueInfo.queueFamilyIndex = m_QueueFamilyIndices.Graphics;
			queueInfo.queueCount = 1;
			queueInfo.pQueuePriorities = &defaultQueuePriority;
			m_QueueCreateInfos.push_back(queueInfo);
		}

		// Dedicated compute queue
		if (requestedQueueTypes & VK_QUEUE_COMPUTE_BIT)
		{
			if (m_QueueFamilyIndices.Compute != m_QueueFamilyIndices.Graphics)
			{
				// If compute family index differs, we need an additional queue create info for the compute queue
				VkDeviceQueueCreateInfo queueInfo = {};
				queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				queueInfo.queueFamilyIndex = m_QueueFamilyIndices.Compute;
				queueInfo.queueCount = 1;
				queueInfo.pQueuePriorities = &defaultQueuePriority;
				m_QueueCreateInfos.push_back(queueInfo);
			}
		}

		// Dedicated transfer queue
		if (requestedQueueTypes & VK_QUEUE_TRANSFER_BIT)
		{
			if ((m_QueueFamilyIndices.Transfer != m_QueueFamilyIndices.Graphics) && (m_QueueFamilyIndices.Transfer != m_QueueFamilyIndices.Compute))
			{
				// If compute family index differs, we need an additional queue create info for the compute queue
				VkDeviceQueueCreateInfo queueInfo{};
				queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				queueInfo.queueFamilyIndex = m_QueueFamilyIndices.Transfer;
				queueInfo.queueCount = 1;
				queueInfo.pQueuePriorities = &defaultQueuePriority;
				m_QueueCreateInfos.push_back(queueInfo);
			}
		}
	}

	bool VKPhysicalDevice::IsExtensionSupported(const std::string& extensionName) const
	{
		return m_SupportedExtensions.find(extensionName) != m_SupportedExtensions.end();
	}

	VKPhysicalDevice::PhysicalDeviceInfo VKPhysicalDevice::GetInfo(VkPhysicalDevice device)
	{
		VkPhysicalDeviceProperties properties = {};
		vkGetPhysicalDeviceProperties(device, &properties);

		VkPhysicalDeviceMemoryProperties memoryProperties = {};
		vkGetPhysicalDeviceMemoryProperties(device, &memoryProperties);

		uint64_t memory = static_cast<uint64_t>(memoryProperties.memoryHeaps[0].size);
		uint32_t memoryMB = static_cast<uint32_t>(memory / 1024 / 1024);

		VKPhysicalDevice::PhysicalDeviceInfo info = {};
		info.Name = std::string(properties.deviceName);
		info.VendorID = properties.vendorID;
		info.Memory = memoryMB;
		info.Handle = device;

		return info;
	}

	VKPhysicalDevice::QueueFamilyIndices VKPhysicalDevice::GetQueueFamilyIndices(int flags)
	{
		QueueFamilyIndices indices;

		// Dedicated queue for compute
		// Try to find a queue family index that supports compute but not graphics
		if (flags & VK_QUEUE_COMPUTE_BIT)
		{
			for (uint32_t i = 0; i < m_QueueFamilyProperties.size(); i++)
			{
				auto& queueFamilyProperties = m_QueueFamilyProperties[i];
				if ((queueFamilyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT) && ((queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0))
				{
					indices.Compute = i;
					break;
				}
			}
		}

		// Dedicated queue for transfer
		// Try to find a queue family index that supports transfer but not graphics and compute
		if (flags & VK_QUEUE_TRANSFER_BIT)
		{
			for (uint32_t i = 0; i < m_QueueFamilyProperties.size(); i++)
			{
				auto& queueFamilyProperties = m_QueueFamilyProperties[i];
				if ((queueFamilyProperties.queueFlags & VK_QUEUE_TRANSFER_BIT) && ((queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) && ((queueFamilyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT) == 0))
				{
					indices.Transfer = i;
					break;
				}
			}
		}

		// For other queue types or if no separate compute queue is present, return the first one to support the requested flags
		for (uint32_t i = 0; i < m_QueueFamilyProperties.size(); i++)
		{
			if ((flags & VK_QUEUE_TRANSFER_BIT) && indices.Transfer == -1)
			{
				if (m_QueueFamilyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
					indices.Transfer = i;
			}

			if ((flags & VK_QUEUE_COMPUTE_BIT) && indices.Compute == -1)
			{
				if (m_QueueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
					indices.Compute = i;
			}

			if (flags & VK_QUEUE_GRAPHICS_BIT)
			{
				if (m_QueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
					indices.Graphics = i;
			}
		}

		return indices;
	}

}