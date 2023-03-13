#pragma once
#include "VK.h"
#include "TSingleton.h"
#include "VKPhysicalDevice.h"

#include <shared_mutex>

namespace Lumos
{

	class VKDevice :public ThreadSafeSingleton<VKDevice>
	{
		friend class ThreadSafeSingleton<VKDevice>;
	public:
		VKDevice();
		~VKDevice();

		bool Init();

		VkDevice GetDevice() const
		{
			return m_Device;
		}

		VkQueue GetGraphicsQueue() const
		{
			return m_GraphicsQueue;
		}

		VkQueue GetPresentQueue() const
		{
			return m_PresentQueue;
		}

		VkQueue GetComputeQueue() const
		{
			return m_ComputeQueue;
		}

		static VkDevice GetHandle()
		{
			return VKDevice::Get().GetDevice();
		}

		const VkPhysicalDeviceFeatures& GetEnabledFeatures()
		{
			return m_EnabledFeatures;
		}

		const std::shared_ptr<VKPhysicalDevice>& GetPhysicalDevice() const
		{
			return m_PhysicalDevice;
		}

	private:
		VkDevice m_Device;

		VkQueue m_ComputeQueue;
		VkQueue m_GraphicsQueue;
		VkQueue m_PresentQueue;

		VkPipelineCache m_PipelineCache;
		VkDescriptorPool m_DescriptorPool;
		VkPhysicalDeviceFeatures m_EnabledFeatures;


		std::shared_ptr<VKPhysicalDevice> m_PhysicalDevice;

		bool m_EnableDebugMarkers = false;

		static uint32_t s_GraphicsQueueFamilyIndex;
	};

}

