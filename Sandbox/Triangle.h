// 创建三角形

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

// 顶点数据结构体
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

	// 结构体数据的绑定说明
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

// 交换链支持详情
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

	//创建Vulkan实例
	void createInstance();
	//选定好物理设备
	void pickPhysicalDevice();
	//选取逻辑设备，并创建相关队列
	void createLogicalDevice();
	//创建surface
	void createSurface();
	//创建交换链
	void createSwapChain();
	//旧的交换链的清理
	void cleanupSwapChain();
	//重新创建交换链
	void recreateSwapChain();
	//创建VkImageViews用于使用交换链上面的VkImage
	void createImageViews();
	// 创建渲染通道
	void createRenderPass();
	//创建渲染管线
	void createGraphicsPipeline();
	//创建帧缓冲区
	void createFramebuffers();
	//创建命令池
	void createCommandPool();
	//创建命令缓冲区
	void createCommandBuffers();
	//命令缓冲区记录
	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
	//渲染与演示
	void drawFrame();
	//创建同步对象
	void createSyncObjects();

	// 载入模型
	void loadModel();

	//创建顶点缓冲
	void createVertexBuffer();
	//创建顶点索引缓冲区
	void createIndexBuffer();
	// 创建统一描述符缓冲区
	void createUniformBuffers();
	
	//缓冲区的创建
	void createBuffer(VkDeviceSize size,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags properties,
		VkBuffer& buffer,
		VkDeviceMemory& bufferMemory);
	// 缓冲区的复制
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	//创建描述符池
	void createDescriptorPool();
	//创建描述符布局
	void createDescriptorSetLayout();
	//创建描述符集
	void createDescriptorSets();

	//更新描述符数据
	void updateUniformBuffer(uint32_t currentImage);

	//创建纹理贴图
	void createTextureImage();
	//创建纹理采样器
	void createTextureSampler();

	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);

	//创建纹理图像视图
	void createTextureImageView();
	//创建深度图像和视图
	void createDepthResources();

	//创建图像
	void createImage(uint32_t width, 
		uint32_t height, 
		VkFormat format, 
		VkImageTiling tiling, 
		VkImageUsageFlags usage, 
		VkMemoryPropertyFlags properties, 
		VkImage& image, 
		VkDeviceMemory& imageMemory);
	//图像的布局转换
	void transitionImageLayout(VkImage image, 
		VkFormat format, 
		VkImageLayout oldLayout, 
		VkImageLayout newLayout);

	// 将缓冲区复制到图像
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

	//验证层是否可用
	bool checkValidationLayerSupport();
	//选取合适的物理设备
	bool isDeviceSuitable(VkPhysicalDevice device);
	// 设备的队列族信息
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
	//检查设备的扩展支持情况
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	// 查询交换链的支持详情
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	// 选取format和颜色空间
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	// 选取展现的模式
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	// 选取交换范围
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	// 创建着色器模块
	VkShaderModule createShaderModule(const std::vector<char>& code);
	// 查询内存要求
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	//辅助函数
	VkCommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(VkCommandBuffer commandBuffer);

	//用于深度视图的创建
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

	//交换链相关
	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

	// 用于使用VkImage
	std::vector<VkImageView> swapChainImageViews;

	//	渲染通道
	VkRenderPass renderPass;
	// 描述符布局
	VkDescriptorSetLayout descriptorSetLayout;
	// 渲染管线布局
	VkPipelineLayout pipelineLayout;
	// 图形管线
	VkPipeline graphicsPipeline;

	std::vector<VkFramebuffer> swapChainFramebuffers;
	// 命令缓冲池
	VkCommandPool commandPool;
	// 命令缓冲区
	std::vector<VkCommandBuffer> commandBuffers;

	// 信号量
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	// 栅栏
	std::vector<VkFence> inFlightFences;

	// 顶点缓冲区
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;

	// 顶点索引缓冲区
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;

	// 描述符缓冲区
	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;
	std::vector<void*> uniformBuffersMapped;

	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets;

	//纹理图像
	VkImage textureImage;
	VkDeviceMemory textureImageMemory;
	// 纹理图像视图
	VkImageView textureImageView;
	// 纹理采样
	VkSampler textureSampler;

	// 深度缓冲相关
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

	const bool enableValidationLayers = true;		//启用验证层消息

	const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	// 同时渲染的帧的数量
	const int MAX_FRAMES_IN_FLIGHT = 2;
	// 用于同时渲染多个帧
	uint32_t currentFrame = 0;

	//// 顶点信息
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

	//// 顶点索引信息
	//const std::vector<uint16_t> indices = {
	//0, 1, 2, 2, 3, 0,
	//4, 5, 6, 6, 7, 4
	//};

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	const std::string MODEL_PATH = "../models/viking_room.obj";
	const std::string TEXTURE_PATH = "../textures/viking_room.png";
};