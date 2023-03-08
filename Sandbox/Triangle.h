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
	void drawFrame(Timestep timestep);
	//����ͬ������
	void createSyncObjects();

	// ����ģ��
	void loadModel();
	void capsule_loadModel();
	void capsule_createVertexBuffer();
	void capsule_createIndexBuffer();
	void capsule_createUniformBuffers();
	void capsule_updateUniformBuffer(uint32_t currentImage);




	//�������㻺��
	void createVertexBuffer();
	//������������������
	void createIndexBuffer();
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

	//���ز���
	VkImage colorImage;
	VkDeviceMemory colorImageMemory;
	VkImageView colorImageView;

	// ��Ȼ������
	VkImage depthImage;
	VkDeviceMemory depthImageMemory;
	VkImageView depthImageView;


	Texture m_texture;
	Texture m_texture1;


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

	//const std::string MODEL_PATH = "../models/viking_room.obj";
	//const std::string TEXTURE_PATH = "../textures/viking_room.png";

	const std::string CAPSULE_MODEL_PATH = "../models/Capsule.obj";
	const std::string CAPSULE_TEXTURE_PATH = "../textures/Capsule.jpg";

	//const std::string CAPSULE_MODEL_PATH = "../models/CrashCar.obj";
	//const std::string CAPSULE_TEXTURE_PATH = "../textures/CrashCar.png";
	std::vector<Vertex> capsule_vertices;
	std::vector<uint32_t> capsule_indices;
	// ���㻺����
	VkBuffer capsule_vertexBuffer;
	VkDeviceMemory capsule_vertexBufferMemory;

	// ��������������
	VkBuffer capsule_indexBuffer;
	VkDeviceMemory capsule_indexBufferMemory;

	std::vector<VkBuffer> capsule_uniformBuffers;
	std::vector<VkDeviceMemory> capsule_uniformBuffersMemory;
	std::vector<void*> capsule_uniformBuffersMapped;

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
	glm::vec3 mCamera_Up = glm::vec3(0.0f, 1.0f, 0.0f);


	float m_aspect = 45.0f;

	bool keys[1024];

	float m_LastFrameTime = 0.0f;
	float camerSpeed = 20.0f;

public:
	// ȫ�ֱ���
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device;
	VkQueue graphicsQueue;
	VkDescriptorPool descriptorPool;
	VkCommandPool commandPool;
	VkDescriptorSetLayout descriptorSetLayout;
};