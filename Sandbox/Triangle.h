// ����������

#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <optional>

#include <glm/glm.hpp>
#include <array>



// �������ݽṹ��
struct Vertex
{
	glm::vec2 pos;
	glm::vec3 color;

	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);
		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);
		return attributeDescriptions;
	}

	// �ṹ�����ݵİ�˵��
	static VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescription;
	}
};


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
	//�ɵĽ�����������
	void cleanupSwapChain();
	//���´���������
	void recreateSwapChain();
	//����VkImageViews����ʹ�ý����������VkImage
	void createImageViews();
	// ������Ⱦͨ��
	void createRenderPass();
	//������Ⱦ����
	void createGraphicsPipeline();
	//����֡������
	void createFramebuffers();
	//���������
	void createCommandPool();
	//�����������
	void createCommandBuffers();
	//���������¼
	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
	//��Ⱦ����ʾ
	void drawFrame();
	//����ͬ������
	void createSyncObjects();


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
	// ������ɫ��ģ��
	VkShaderModule createShaderModule(const std::vector<char>& code);


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

	// ����ʹ��VkImage
	std::vector<VkImageView> swapChainImageViews;

	//	��Ⱦͨ��
	VkRenderPass renderPass;
	// ��Ⱦ���߲���
	VkPipelineLayout pipelineLayout;
	// ͼ�ι���
	VkPipeline graphicsPipeline;

	std::vector<VkFramebuffer> swapChainFramebuffers;
	// ������
	VkCommandPool commandPool;
	// �������
	std::vector<VkCommandBuffer> commandBuffers;

	// �ź���
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	// դ��
	std::vector<VkFence> inFlightFences;

public:
	bool framebufferResized = false;

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

	// ͬʱ��Ⱦ��֡������
	const int MAX_FRAMES_IN_FLIGHT = 2;
	// ����ͬʱ��Ⱦ���֡
	uint32_t currentFrame = 0;

	const std::vector<Vertex> vertices = {
	{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
	{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
	{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
	};

};