// 创建三角形

#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

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

#include "Timer.h"
#include "Timestep.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include <stb_image.h>

#include "Texture.h"
#include "Model.h"

		/// Return the smaller of two values.
template <class T, class U>
inline T Min(T lhs, U rhs)
{
	return lhs < rhs ? lhs : rhs;
}

/// Return the larger of two values.
template <class T, class U>
inline T Max(T lhs, U rhs)
{
	return lhs > rhs ? lhs : rhs;
}

struct UniformBufferObject
{
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;
	bool isComplete() {
		return graphicsFamily.has_value()
			&& presentFamily.has_value();

	}
};

struct Color4f
{
	Color4f(float cr, float cg, float cb, float ca)
	{
		r = cr;
		g = cg;
		b = cb;
		a = ca;
	}
	// normalized [0-1]
	float r, g, b, a;
};

struct PushConstant
{
	glm::vec3 alignas(16) color;
	glm::vec3 alignas(16) direction;
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
			return (((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec3>()(vertex.color) << 1) >> 1) ^ (hash<glm::vec2>()(vertex.uv) << 1);
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
	void drawFrame(Timestep timestep);
	//创建同步对象
	void createSyncObjects();

	void capsule_createUniformBuffers();
	void capsule_updateUniformBuffer(uint32_t currentImage);


	// 创建统一描述符缓冲区
	void createUniformBuffers();

	//创建描述符池
	void createDescriptorPool();
	//创建描述符布局
	void createDescriptorSetLayout();
	//创建描述符集
	void createDescriptorSets();

	//更新描述符数据
	void updateUniformBuffer(uint32_t currentImage);

	//创建深度图像和视图
	void createDepthResources();
	//创建多重采样资源
	void createColorResources();

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

	//用于深度视图的创建
	VkFormat findDepthFormat();

	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates,
		VkImageTiling tiling,
		VkFormatFeatureFlags features);

	//获得最大多重采样数量
	VkSampleCountFlagBits getMaxUsableSampleCount();
private:
	GLFWwindow* window;

	VkInstance instance;

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

	// 命令缓冲区
	std::vector<VkCommandBuffer> commandBuffers;

	// 信号量
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	// 栅栏
	std::vector<VkFence> inFlightFences;

	// 描述符缓冲区
	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;
	std::vector<void*> uniformBuffersMapped;

	//多重采样
	VkImage colorImage;
	VkDeviceMemory colorImageMemory;
	VkImageView colorImageView;

	// 深度缓冲相关
	VkImage depthImage;
	VkDeviceMemory depthImageMemory;
	VkImageView depthImageView;

	std::vector<VkDescriptorSet> descriptorSets;
	Texture m_texture;
	Texture m_texture1;

	Model model;
	Model model1;

	// 渲染管线布局
	VkPipelineLayout color_pipelineLayout;
	// 图形管线
	VkPipeline color_graphicsPipeline;


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

	//const std::string MODEL_PATH = "../models/viking_room.obj";
	//const std::string TEXTURE_PATH = "../textures/viking_room.png";

	const std::string CAPSULE_MODEL_PATH = "../models/smooth_vase.obj";
	const std::string CAPSULE_TEXTURE_PATH = "../textures/Capsule.jpg";

	//const std::string CAPSULE_MODEL_PATH = "../models/CrashCar.obj";
	//const std::string CAPSULE_TEXTURE_PATH = "../textures/CrashCar.png";


	std::vector<VkBuffer> capsule_uniformBuffers;
	std::vector<VkDeviceMemory> capsule_uniformBuffersMemory;
	std::vector<void*> capsule_uniformBuffersMapped;
	std::vector<VkDescriptorSet> capsule_descriptorSets;

	const std::string MODEL_PATH = "../models/CrashCar.obj";
	const std::string TEXTURE_PATH = "../textures/CrashCar.png";

	// 多重采样
	VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;

public:
	void initImgui();
	void imguiRender(VkCommandBuffer commandBuffer);
	void clearImgui();

	// Imgui 控件绘制
public:
	bool PorpertyTransform(const std::string& name, glm::vec3& vector, float width);
	void SetLocalMatrix();
	void cameraMove(Timestep ts);
	void timeController();
public:
	glm::mat4 m_LocalMatrix = glm::mat4(1);

	glm::mat4 capsule_m_LocalMatrix = glm::mat4(1);


	glm::vec3 m_Position = glm::vec3(0);
	glm::vec3 m_Rotation = glm::vec3(0);
	glm::vec3 m_Scale = glm::vec3(1);

	//glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)
	glm::vec3 mCamera_Position = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 mCamera_Target = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 mCamera_Up = glm::vec3(0.0f, -1.0f, 0.0f);


	float m_aspect = 45.0f;

	bool keys[1024];

	float m_LastFrameTime = 0.0f;
	float camerSpeed = 1.0f;

	Color4f colorEditor = Color4f(1.0f, 0.0f, 0.0f, 0.0f);
	float lightDirection[3] = { 0,0,1 };


	PushConstant pushConstant;

public:
	// 全局变量
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device;
	VkQueue graphicsQueue;
	VkDescriptorPool descriptorPool;
	VkCommandPool commandPool;
	VkDescriptorSetLayout uniformDescriptorSetLayout;
	VkDescriptorSetLayout textureDescriptorSetLayout;



};