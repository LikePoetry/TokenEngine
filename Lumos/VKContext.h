#pragma once
#include "VK.h"
#include <vector>


const bool EnableValidationLayers = true;

namespace Lumos
{
	class VKContext
	{
	public:
		VKContext();
		~VKContext();

		void Init();

		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugReportFlagsEXT flags,
			VkDebugReportObjectTypeEXT objType,
			uint64_t sourceObj,
			size_t location,
			int32_t msgCode,
			const char* pLayerPrefix,
			const char* pMsg,
			void* userData);

		static VkInstance GetVkInstance() { return s_VkInstance; }

		bool GetEnableValidationLayers() { return EnableValidationLayers; }
		const std::vector<const char*>& GetLayerNames() const { return m_InstanceLayerNames; }
		const std::vector<const char*>& GetExtensionNames() const { return m_InstanceExtensionNames; }


	protected:
		void CreateInstance();
		void SetupDebugCallback();
		bool CheckValidaionLayerSupport(std::vector<const char*>& validationLayers);
		bool CheckExtensionSupport(std::vector<const char*>& extensions);

		static const std::vector<const char*> GetRequiredExtensions();
		const std::vector<const char*> GetRequiredLayers() const;

	private:
		static VkInstance s_VkInstance;
		VkDebugReportCallbackEXT m_DebugCallback = VK_NULL_HANDLE;

		std::vector<VkLayerProperties> m_InstanceLayers;
		std::vector<VkExtensionProperties> m_InstanceExtensions;

		std::vector<const char*> m_InstanceLayerNames;
		std::vector<const char*> m_InstanceExtensionNames;

		static uint32_t m_VKVersion;
	};
}