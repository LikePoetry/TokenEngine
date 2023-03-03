// 创建三角形

#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <optional>


struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;
	bool isComplete() {
		return graphicsFamily.has_value()
			&& presentFamily.has_value();

	}
};

class Triangle
{
public:
	Triangle();
	~Triangle();

	void run();

private:
	void initWindow();
	void initVulkan();
	void mainLoop();
	void cleanUp();

	//创建Vulkan实例
	void createInstance();
	//选定好物理设备
	void pickPhysicalDevice();
	//选取逻辑设备，并创建相关队列
	void createLogicalDevice();
	//创建surface
	void createSurface();


	//验证层是否可用
	bool checkValidationLayerSupport();
	//选取合适的物理设备
	bool isDeviceSuitable(VkPhysicalDevice device);
	// 设备的队列族信息
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
private:
	GLFWwindow* window;

	VkInstance instance;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device;

	VkQueue graphicsQueue;
	VkQueue presentQueue;

	VkSurfaceKHR surface;

private:
	const uint32_t WIDTH = 800;
	const uint32_t HEIGHT = 600;

	const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
	};

	const bool enableValidationLayers = true;		//启用验证层消息

};