// 创建三角形

#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <optional>

#include <glm/glm.hpp>
#include <array>



// 顶点数据结构体
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

	// 结构体数据的绑定说明
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

// 交换链支持详情
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
	//创建顶点缓冲
	void createVertexBuffer();
	//缓冲区的创建
	void createBuffer(VkDeviceSize size,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags properties,
		VkBuffer& buffer,
		VkDeviceMemory& bufferMemory);
	// 缓冲区的复制
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);


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

	const std::vector<Vertex> vertices = {
	{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
	{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
	{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
	};

};