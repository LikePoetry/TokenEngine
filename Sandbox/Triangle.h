// ����������

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
	void drawFrame(Timestep timestep);
	//����ͬ������
	void createSyncObjects();

	void capsule_createUniformBuffers();
	void capsule_updateUniformBuffer(uint32_t currentImage);


	// ����ͳһ������������
	void createUniformBuffers();

	//������������
	void createDescriptorPool();
	//��������������
	void createDescriptorSetLayout();
	//������������
	void createDescriptorSets();

	//��������������
	void updateUniformBuffer(uint32_t currentImage);

	//�������ͼ�����ͼ
	void createDepthResources();
	//�������ز�����Դ
	void createColorResources();

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

	//���������ͼ�Ĵ���
	VkFormat findDepthFormat();

	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates,
		VkImageTiling tiling,
		VkFormatFeatureFlags features);

	//��������ز�������
	VkSampleCountFlagBits getMaxUsableSampleCount();
private:
	GLFWwindow* window;

	VkInstance instance;

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

	// �������
	std::vector<VkCommandBuffer> commandBuffers;

	// �ź���
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	// դ��
	std::vector<VkFence> inFlightFences;

	// ������������
	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;
	std::vector<void*> uniformBuffersMapped;

	//���ز���
	VkImage colorImage;
	VkDeviceMemory colorImageMemory;
	VkImageView colorImageView;

	// ��Ȼ������
	VkImage depthImage;
	VkDeviceMemory depthImageMemory;
	VkImageView depthImageView;

	std::vector<VkDescriptorSet> descriptorSets;
	Texture m_texture;
	Texture m_texture1;

	Model model;
	Model model1;

	// ��Ⱦ���߲���
	VkPipelineLayout color_pipelineLayout;
	// ͼ�ι���
	VkPipeline color_graphicsPipeline;


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

	//const std::string MODEL_PATH = "../models/viking_room.obj";
	//const std::string TEXTURE_PATH = "../textures/viking_room.png";

	const std::string CAPSULE_MODEL_PATH = "../models/flat_vase.obj";
	const std::string CAPSULE_TEXTURE_PATH = "../textures/Capsule.jpg";

	//const std::string CAPSULE_MODEL_PATH = "../models/CrashCar.obj";
	//const std::string CAPSULE_TEXTURE_PATH = "../textures/CrashCar.png";


	std::vector<VkBuffer> capsule_uniformBuffers;
	std::vector<VkDeviceMemory> capsule_uniformBuffersMemory;
	std::vector<void*> capsule_uniformBuffersMapped;
	std::vector<VkDescriptorSet> capsule_descriptorSets;

	const std::string MODEL_PATH = "../models/CrashCar.obj";
	const std::string TEXTURE_PATH = "../textures/CrashCar.png";

	// ���ز���
	VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;

public:
	void initImgui();
	void imguiRender(VkCommandBuffer commandBuffer);
	void clearImgui();

	// Imgui �ؼ�����
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
	glm::vec3 mCamera_Position = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 mCamera_Target = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 mCamera_Up = glm::vec3(0.0f, -1.0f, 0.0f);


	float m_aspect = 45.0f;

	bool keys[1024];

	float m_LastFrameTime = 0.0f;
	float camerSpeed = 20.0f;

	Color4f colorEditor = Color4f(1.0f, 0.0f, 0.0f, 0.0f);
	float lightDirection[3];


	PushConstant pushConstant;

public:
	// ȫ�ֱ���
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device;
	VkQueue graphicsQueue;
	VkDescriptorPool descriptorPool;
	VkCommandPool commandPool;
	VkDescriptorSetLayout uniformDescriptorSetLayout;
	VkDescriptorSetLayout textureDescriptorSetLayout;



};