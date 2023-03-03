// ����������

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

// ������֧������
struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
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

	//����Vulkanʵ��
	void createInstance();
	//ѡ���������豸
	void pickPhysicalDevice();
	//ѡȡ�߼��豸����������ض���
	void createLogicalDevice();
	//����surface
	void createSurface();
	//����������
	void createSwapChain();


	//��֤���Ƿ����
	bool checkValidationLayerSupport();
	//ѡȡ���ʵ������豸
	bool isDeviceSuitable(VkPhysicalDevice device);
	// �豸�Ķ�������Ϣ
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
	//����豸����չ֧�����
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	// ��ѯ��������֧������
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	// ѡȡformat����ɫ�ռ�
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	// ѡȡչ�ֵ�ģʽ
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	// ѡȡ������Χ
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

private:
	GLFWwindow* window;

	VkInstance instance;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device;

	VkQueue graphicsQueue;
	VkQueue presentQueue;

	VkSurfaceKHR surface;

	//���������
	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

private:
	const uint32_t WIDTH = 800;
	const uint32_t HEIGHT = 600;

	const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
	};

	const bool enableValidationLayers = true;		//������֤����Ϣ

	const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

};