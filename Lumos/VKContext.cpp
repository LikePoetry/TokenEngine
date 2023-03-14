#include "VKContext.h"

#define VK_LAYER_LUNARG_VALIDATION_NAME "VK_LAYER_KHRONOS_validation"

namespace Lumos
{

	VkResult CreateDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback)
	{
		auto func = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT"));

		if (func != nullptr)
		{
			return func(instance, pCreateInfo, pAllocator, pCallback);
		}
		else
		{
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	VkInstance VKContext::s_VkInstance = nullptr;

	VKContext::VKContext()
	{

	}

	VKContext::~VKContext()
	{
		vkDestroyInstance(s_VkInstance, nullptr);
	}

	void VKContext::Init()
	{
		CreateInstance();
		//SetupDebugCallback();
	}

	void VKContext::CreateInstance()
	{
		m_InstanceLayerNames = GetRequiredLayers();
		m_InstanceExtensionNames = GetRequiredExtensions();

		if (!CheckValidaionLayerSupport(m_InstanceLayerNames))
		{
			// layers not available;
		}

		if (!CheckExtensionSupport(m_InstanceExtensionNames))
		{
			// extensions not available;
		}

		VkApplicationInfo appInfo = {};

		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Hello Triangle";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo createInfo = {};
		createInfo.pApplicationInfo = &appInfo;
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(m_InstanceExtensionNames.size());
		createInfo.ppEnabledExtensionNames = m_InstanceExtensionNames.data();
		createInfo.enabledLayerCount = static_cast<uint32_t>(m_InstanceLayerNames.size());
		createInfo.ppEnabledLayerNames = m_InstanceLayerNames.data();
		//createInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

		VkResult createResult = vkCreateInstance(&createInfo, nullptr, &s_VkInstance);
		if (createResult != VK_SUCCESS)
		{
			// Failed to create instance;
		}
	}


	VkBool32  VKContext::DebugCallback(VkDebugReportFlagsEXT flags,
		VkDebugReportObjectTypeEXT objType,
		uint64_t sourceObj,
		size_t location,
		int32_t msgCode,
		const char* pLayerPrefix,
		const char* pMsg,
		void* userData)
	{
		// TODO
		if (!flags)
			return VK_FALSE;

		if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
		{
		}
		// Warnings may hint at unexpected / non-spec API usage
		if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
		{
		}
		// May indicate sub-optimal usage of the API
		if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
		{
		}
		// Informal messages that may become handy during debugging
		if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
		{
		}
		// Diagnostic info from the Vulkan loader and layers
		// Usually not helpful in terms of API usage, but may help to debug layer and loader problems
		if (flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT)
		{
		}

		return VK_FALSE;
	}

	void VKContext::SetupDebugCallback()
	{
		if (!EnableValidationLayers)
		{
			return;
		}

		VkDebugReportCallbackCreateInfoEXT createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
		createInfo.pfnCallback = reinterpret_cast<PFN_vkDebugReportCallbackEXT>(DebugCallback);

		VkResult result = CreateDebugReportCallbackEXT(s_VkInstance, &createInfo, nullptr, &m_DebugCallback);
		if (result != VK_SUCCESS)
		{
			// "[VULKAN] Failed to set up debug callback!"
		}
	}

	const std::vector<const char*> VKContext::GetRequiredExtensions()
	{
		std::vector<const char*> extensions;

		if (EnableValidationLayers)
		{
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
			extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
			extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
		}

		extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
		extensions.push_back("VK_KHR_portability_enumeration");

		extensions.push_back("VK_KHR_win32_surface");
		return extensions;

	}

	const std::vector<const char*> VKContext::GetRequiredLayers() const
	{
		std::vector<const char*> layers;

		if (EnableValidationLayers)
		{
			layers.emplace_back(VK_LAYER_LUNARG_VALIDATION_NAME);
		}

		return layers;
	}

	bool VKContext::CheckValidaionLayerSupport(std::vector<const char*>& validationLayers)
	{
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		m_InstanceLayers.resize(layerCount);

		vkEnumerateInstanceLayerProperties(&layerCount, m_InstanceLayers.data());
		bool removedLayer = false;

		validationLayers.erase(
			std::remove_if(
				validationLayers.begin(),
				validationLayers.end(),
				[&](const char* layerName)
				{
					bool layerFound = false;

		for (const auto& layerProperties : m_InstanceLayers)
		{
			if (strcmp(layerName, layerProperties.layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}

		if (!layerFound)
		{
			removedLayer = true;
		}

		return !layerFound;
				}),
			validationLayers.end());

		return !removedLayer;
	}

	bool VKContext::CheckExtensionSupport(std::vector<const char*>& extensions)
	{
		uint32_t extensionCount;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

		m_InstanceExtensions.resize(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, m_InstanceExtensions.data());

		bool removedExtension = false;

		extensions.erase(
			std::remove_if(
				extensions.begin(),
				extensions.end(),
				[&](const char* extensionName)
				{
					bool extensionFound = false;

		for (const auto& extensionProperties : m_InstanceExtensions)
		{
			if (strcmp(extensionName, extensionProperties.extensionName) == 0)
			{
				extensionFound = true;
				break;
			}
		}

		if (!extensionFound)
		{
			removedExtension = true;
		}

		return !extensionFound;
				}),
			extensions.end());

		return !removedExtension;
	}

}
