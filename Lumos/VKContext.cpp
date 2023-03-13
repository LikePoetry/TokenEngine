#include "VKContext.h"

#define VK_LAYER_LUNARG_VALIDATION_NAME "VK_LAYER_KHRONOS_validation"

namespace Lumos
{

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
