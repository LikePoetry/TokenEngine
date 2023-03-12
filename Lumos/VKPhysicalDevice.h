#pragma once

#include "VK.h"
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace Lumos
{
	class VKPhysicalDevice
	{
		public:
			VKPhysicalDevice();
			~VKPhysicalDevice();


			struct PhysicalDeviceInfo
			{
				uint32_t Memory;
				uint32_t VendorID;
				std::string Driver;
				std::string APIVersion;
				std::string Vendor;
				std::string Name;

				VkPhysicalDevice Handle;
			};

			bool IsExtensionSupported(const std::string& extensionName) const;
			uint32_t GetMemoryTypeIndex(uint32_t typeBits, VkMemoryPropertyFlags properties) const;

			PhysicalDeviceInfo GetInfo(VkPhysicalDevice device);

			VkPhysicalDevice GetHandle() const
			{
				return m_Handle;
			}

			int32_t GetGraphicsQueueFamilyIndex()
			{
				return m_QueueFamilyIndices.Graphics;
			}

			VkPhysicalDeviceProperties GetProperties() const
			{
				return m_PhysicalDeviceProperties;
			}

			VkPhysicalDeviceMemoryProperties GetMemoryProperties() const
			{
				return m_MemoryProperties;
			}

	private:

		struct QueueFamilyIndices
		{
			int32_t Graphics = -1;
			int32_t Compute = -1;
			int32_t Transfer = -1;
		};

		QueueFamilyIndices GetQueueFamilyIndices(int queueFlags);

		uint32_t GetGPUCount() const
		{
			return m_GPUCount;
		}

	private:

		QueueFamilyIndices m_QueueFamilyIndices;

		std::vector<VkQueueFamilyProperties> m_QueueFamilyProperties;
		std::unordered_set<std::string> m_SupportedExtensions;
		std::vector<VkDeviceQueueCreateInfo> m_QueueCreateInfos;

		VkPhysicalDevice m_Handle;
		VkPhysicalDeviceProperties m_PhysicalDeviceProperties;
		VkPhysicalDeviceMemoryProperties m_MemoryProperties;

		uint32_t m_GPUCount = 0;
		PhysicalDeviceInfo m_DeviceInfo;

		friend class VKDevice;
	};
}