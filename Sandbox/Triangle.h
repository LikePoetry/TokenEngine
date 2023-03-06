// ����������

#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <optional>
#include <iostream>
#include <unordered_map>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include <array>



struct UniformBufferObject
{
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};

// �������ݽṹ��
struct Vertex
{
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 texCoord;

	static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, texCoord);
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

	bool operator==(const Vertex& other) const {
		return pos == other.pos && color == other.color && texCoord == other.texCoord;
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

namespace std {
	template<> struct hash<Vertex> {
		size_t operator()(Vertex const& vertex) const {
			return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
		}
	};
}

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

	// ����ģ��
	void loadModel();

	//�������㻺��
	void createVertexBuffer();
	//������������������
	void createIndexBuffer();
	// ����ͳһ������������
	void createUniformBuffers();
	
	//�������Ĵ���
	void createBuffer(VkDeviceSize size,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags properties,
		VkBuffer& buffer,
		VkDeviceMemory& bufferMemory);
	// �������ĸ���
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	//������������
	void createDescriptorPool();
	//��������������
	void createDescriptorSetLayout();
	//������������
	void createDescriptorSets();

	//��������������
	void updateUniformBuffer(uint32_t currentImage);

	//����������ͼ
	void createTextureImage();
	//�������������
	void createTextureSampler();

	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);

	//��������ͼ����ͼ
	void createTextureImageView();
	//�������ͼ�����ͼ
	void createDepthResources();

	//����ͼ��
	void createImage(uint32_t width, 
		uint32_t height, 
		VkFormat format, 
		VkImageTiling tiling, 
		VkImageUsageFlags usage, 
		VkMemoryPropertyFlags properties, 
		VkImage& image, 
		VkDeviceMemory& imageMemory);
	//ͼ��Ĳ���ת��
	void transitionImageLayout(VkImage image, 
		VkFormat format, 
		VkImageLayout oldLayout, 
		VkImageLayout newLayout);

	// �����������Ƶ�ͼ��
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

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
	// ��ѯ�ڴ�Ҫ��
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	//��������
	VkCommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(VkCommandBuffer commandBuffer);

	//���������ͼ�Ĵ���
	VkFormat findDepthFormat();

	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, 
		VkImageTiling tiling, 
		VkFormatFeatureFlags features);
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
	// ����������
	VkDescriptorSetLayout descriptorSetLayout;
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

	// ���㻺����
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;

	// ��������������
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;

	// ������������
	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;
	std::vector<void*> uniformBuffersMapped;

	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets;

	//����ͼ��
	VkImage textureImage;
	VkDeviceMemory textureImageMemory;
	// ����ͼ����ͼ
	VkImageView textureImageView;
	// �������
	VkSampler textureSampler;

	// ��Ȼ������
	VkImage depthImage;
	VkDeviceMemory depthImageMemory;
	VkImageView depthImageView;
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

	//// ������Ϣ
	//const std::vector<Vertex> vertices = {
	// {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
	//{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
	//{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
	//{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

	//{{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
	//{{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
	//{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
	//{{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
	//};

	//// ����������Ϣ
	//const std::vector<uint16_t> indices = {
	//0, 1, 2, 2, 3, 0,
	//4, 5, 6, 6, 7, 4
	//};

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	const std::string MODEL_PATH = "../models/viking_room.obj";
	const std::string TEXTURE_PATH = "../textures/viking_room.png";
};