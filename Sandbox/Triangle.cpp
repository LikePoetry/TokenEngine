// 2023年3月2日 三角形的绘制结构
// 验证层消息，未添加调试信息的回调
// 简单选取物理设备，还可以通过显卡性能等要求进行选取，一般情况下，仅有一个独立显卡！
// 创建逻辑设备，并创建图像队列
// Surface的创建 2023年3月3日14:33:45.创建完成后，需要修改队列，添加上Present Queue;
// 交换链的创建 2023年3月3日16:48:25
// 创建VkImageView 用于使用交换链上面的 VkImage;
// 创建图形管线 操作着色器和固定功能 及 渲染通道;

//描述符布局和缓冲区 
//在管道创建期间指定描述符布局
//从描述符池中分配一个描述符集
//渲染时绑定描述符集

#include "Triangle.h"
#include <iostream>
#include <set>
#include <algorithm>
#include <fstream>
#include <chrono>

#include <memory>



#include <imgui_internal.h>

#include <glm/gtx/matrix_decompose.hpp>
#include <glm/ext.hpp>

#include "VulkanHelper.h"

static std::vector<char> readFile(const std::string& filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("failed to open file!");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}

static void framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	auto app = reinterpret_cast<Triangle*>(glfwGetWindowUserPointer(window));
	app->framebufferResized = true;
}

static void scrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
	auto app = reinterpret_cast<Triangle*>(glfwGetWindowUserPointer(window));
	if (app->m_aspect >= 0.1f && app->m_aspect <= 120.0f)
	{
		app->m_aspect -= yOffset;
	}

	if (app->m_aspect <= 0.1f)
	{
		app->m_aspect = 0.10f;
	}

	if (app->m_aspect >= 120.0f)
	{
		app->m_aspect = 120.0f;
	}
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	auto app = reinterpret_cast<Triangle*>(glfwGetWindowUserPointer(window));
	if (action == GLFW_PRESS)
	{
		app->keys[key] = true;
	}
	else if (action == GLFW_RELEASE)
	{
		app->keys[key] = false;
	}
}

Triangle::Triangle()
{
	memset(keys, false, sizeof(keys));
}

Triangle::~Triangle()
{

}

void Triangle::run()
{
	initWindow();
	initVulkan();
	initImgui();
	mainLoop();
	cleanUp();
}

/// <summary>
/// 初始化GLFW 窗口
/// </summary>
void Triangle::initWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	//glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
	//指定UserPointer;
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetKeyCallback(window, keyCallback);
}

/// <summary>
/// 初始化Vulkan
/// </summary>
void Triangle::initVulkan()
{
	createInstance();
	createSurface();
	pickPhysicalDevice();
	createLogicalDevice();
	createSwapChain();
	createImageViews();

	//------创建渲染相关
	createRenderPass();
	createDescriptorSetLayout();
	createGraphicsPipeline();

	createColorResources();
	createDepthResources();
	// 与绘制相关
	createFramebuffers();

	createCommandPool();

	//model.SetTriangle(this);
	//model.Load(MODEL_PATH.c_str());

	createUniformBuffers();
	//另一个模型

	model1.SetTriangle(this);
	model1.Load(CAPSULE_MODEL_PATH.c_str());

	capsule_createUniformBuffers();

	//创建描述符池
	createDescriptorPool();

	m_texture.SetTriangle(this);
	m_texture1.SetTriangle(this);

	createDescriptorSets();

	//创建 Texture
	m_texture.Load(TEXTURE_PATH.c_str());
	m_texture1.Load(CAPSULE_TEXTURE_PATH.c_str());


	createCommandBuffers();
	createSyncObjects();
}

/// <summary>
/// 创建Vulkan 实例
/// </summary>
void Triangle::createInstance()
{
	if (enableValidationLayers && !checkValidationLayerSupport()) {
		throw std::runtime_error("validation layers requested, but not available!");
	}

	//给驱动程序提供一些可用信息
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	//创建单例
	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	// 检查扩展支持
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	createInfo.enabledExtensionCount = glfwExtensionCount;
	createInfo.ppEnabledExtensionNames = glfwExtensions;

	// 需要启用哪些扩展。
	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;

		createInfo.pNext = nullptr;
	}

	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
		throw std::runtime_error("failed to create instance!");
	}
}

/// <summary>
/// 创建窗口Surface
/// </summary>
void Triangle::createSurface()
{
	if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
		throw std::runtime_error("failed to create window surface!");
	}
}

/// <summary>
/// 创建交换链
/// </summary>
void Triangle::createSwapChain()
{
	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
	{
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
		throw std::runtime_error("failed to create swap chain!");
	}

	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;
}

/// <summary>
/// 重新创建交换链
/// </summary>
void Triangle::recreateSwapChain()
{
	int width = 0, height = 0;
	glfwGetFramebufferSize(window, &width, &height);
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(window, &width, &height);
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(device);

	cleanupSwapChain();

	createSwapChain();
	createImageViews();
	createColorResources();
	createDepthResources();
	createFramebuffers();
}

/// <summary>
/// 旧的交换链清理
/// </summary>
void Triangle::cleanupSwapChain()
{
	vkDestroyImageView(device, depthImageView, nullptr);
	vkDestroyImage(device, depthImage, nullptr);
	vkFreeMemory(device, depthImageMemory, nullptr);

	vkDestroyImageView(device, colorImageView, nullptr);
	vkDestroyImage(device, colorImage, nullptr);
	vkFreeMemory(device, colorImageMemory, nullptr);

	for (size_t i = 0; i < swapChainFramebuffers.size(); i++) {
		vkDestroyFramebuffer(device, swapChainFramebuffers[i], nullptr);
	}

	for (size_t i = 0; i < swapChainImageViews.size(); i++) {
		vkDestroyImageView(device, swapChainImageViews[i], nullptr);
	}

	vkDestroySwapchainKHR(device, swapChain, nullptr);
}

/// <summary>
/// 创建渲染通道
/// </summary>
void Triangle::createRenderPass()
{
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = swapChainImageFormat;
	colorAttachment.samples = msaaSamples;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription depthAttachment{};
	depthAttachment.format = findDepthFormat();
	depthAttachment.samples = msaaSamples;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription colorAttachmentResolve{};
	colorAttachmentResolve.format = swapChainImageFormat;
	colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef{};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference colorAttachmentResolveRef{};
	colorAttachmentResolveRef.attachment = 2;
	colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;
	subpass.pResolveAttachments = &colorAttachmentResolveRef;

	// 子通道依赖,自动处理图像布局转换
	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

	std::array<VkAttachmentDescription, 3> attachments = { colorAttachment, depthAttachment ,colorAttachmentResolve };
	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
		throw std::runtime_error("failed to create render pass!");
	}
}

/// <summary>
/// 创建描述符布局
/// </summary>
void Triangle::createDescriptorSetLayout() {
	VkDescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.pImmutableSamplers = nullptr;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	// 添加纹理采样器描述符集
	VkDescriptorSetLayoutBinding samplerLayoutBinding{};
	samplerLayoutBinding.binding = 0;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;


	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &uboLayoutBinding;

	if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &uniformDescriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}

	layoutInfo.pBindings = &samplerLayoutBinding;

	if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &textureDescriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}

/// <summary>
/// 创建图像管线
/// </summary>
void Triangle::createGraphicsPipeline()
{
	auto vertShaderCode = readFile("../shaders/vert.spv");
	auto fragShaderCode = readFile("../shaders/frag.spv");

	VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
	VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	auto bindingDescription = Vertex::getBindingDescription();
	auto attributeDescriptions = Vertex::getAttributeDescriptions();

	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;

	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_NONE;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = msaaSamples;

	VkPipelineDepthStencilStateCreateInfo depthStencil{};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.stencilTestEnable = VK_FALSE;

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;

	std::vector<VkDynamicState> dynamicStates = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};
	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates = dynamicStates.data();

	VkDescriptorSetLayout SetLayouts[] = { uniformDescriptorSetLayout, textureDescriptorSetLayout };

	VkPushConstantRange psRange;
	psRange.offset = 0;
	psRange.size = sizeof(PushConstant);
	psRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 2;
	pipelineLayoutInfo.pSetLayouts = SetLayouts;
	pipelineLayoutInfo.pushConstantRangeCount = 1;
	pipelineLayoutInfo.pPushConstantRanges = &psRange;


	if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = &depthStencil;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &dynamicState;
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline!");
	}

	//创建另一条管线，在此条管线的基础上进行修改
	auto color_fragShaderCode = readFile("../shaders/frag_color.spv");
	VkShaderModule color_fragShaderModule = createShaderModule(color_fragShaderCode);

	VkPipelineShaderStageCreateInfo color_fragShaderStageInfo{};
	color_fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	color_fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	color_fragShaderStageInfo.module = color_fragShaderModule;
	color_fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo color_shaderStages[] = { vertShaderStageInfo, color_fragShaderStageInfo };



	VkPipelineLayoutCreateInfo color_pipelineLayoutInfo{};
	color_pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	color_pipelineLayoutInfo.setLayoutCount = 1;
	color_pipelineLayoutInfo.pSetLayouts = &uniformDescriptorSetLayout;
	color_pipelineLayoutInfo.pushConstantRangeCount = 1;
	color_pipelineLayoutInfo.pPushConstantRanges = &psRange;

	if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &color_pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}

	VkGraphicsPipelineCreateInfo color_pipelineInfo{};
	color_pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	color_pipelineInfo.stageCount = 2;
	color_pipelineInfo.pStages = color_shaderStages;
	color_pipelineInfo.pVertexInputState = &vertexInputInfo;
	color_pipelineInfo.pInputAssemblyState = &inputAssembly;
	color_pipelineInfo.pViewportState = &viewportState;
	color_pipelineInfo.pRasterizationState = &rasterizer;
	color_pipelineInfo.pMultisampleState = &multisampling;
	color_pipelineInfo.pDepthStencilState = &depthStencil;
	color_pipelineInfo.pColorBlendState = &colorBlending;
	color_pipelineInfo.pDynamicState = &dynamicState;
	color_pipelineInfo.layout = color_pipelineLayout;
	color_pipelineInfo.renderPass = renderPass;
	color_pipelineInfo.subpass = 0;
	color_pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &color_pipelineInfo, nullptr, &color_graphicsPipeline) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline!");
	}




	vkDestroyShaderModule(device, fragShaderModule, nullptr);
	vkDestroyShaderModule(device, vertShaderModule, nullptr);
	vkDestroyShaderModule(device, color_fragShaderModule, nullptr);

}

/// <summary>
/// 创建帧缓冲区
/// </summary>
void Triangle::createFramebuffers()
{
	swapChainFramebuffers.resize(swapChainImageViews.size());

	for (size_t i = 0; i < swapChainImageViews.size(); i++) {
		std::array<VkImageView, 3> attachments = {
			colorImageView,
			depthImageView,
			swapChainImageViews[i]
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = swapChainExtent.width;
		framebufferInfo.height = swapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}

/// <summary>
/// 创建命令池
/// </summary>
void Triangle::createCommandPool()
{
	QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

	if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create command pool!");
	}
}

/// <summary>
/// 创建深度图像和视图
/// </summary>
void Triangle::createDepthResources()
{
	VkFormat depthFormat = findDepthFormat();

	VulkanHelper::createImage(physicalDevice, device, swapChainExtent.width,
		swapChainExtent.height, 1,
		msaaSamples,
		depthFormat,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		depthImage,
		depthImageMemory);
	depthImageView = VulkanHelper::createImageView(device, depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);
}

/// <summary>
/// 创建多重采样资源
/// </summary>
void Triangle::createColorResources()
{
	VkFormat colorFormat = swapChainImageFormat;

	VulkanHelper::createImage(physicalDevice, device, swapChainExtent.width,
		swapChainExtent.height,
		1, msaaSamples, colorFormat,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		colorImage,
		colorImageMemory);
	colorImageView = VulkanHelper::createImageView(device, colorImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
}

VkFormat Triangle::findDepthFormat()
{
	return findSupportedFormat(
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
	);
}

VkFormat Triangle::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
	for (VkFormat format : candidates) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
			return format;
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
			return format;
		}
	}

	throw std::runtime_error("failed to find supported format!");
}

VkSampleCountFlagBits Triangle::getMaxUsableSampleCount()
{
	VkPhysicalDeviceProperties physicalDeviceProperties;
	vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

	VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
	if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
	if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
	if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
	if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
	if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
	if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

	return VK_SAMPLE_COUNT_1_BIT;
}

/// <summary>
/// 创建统一描述符缓冲区
/// </summary>
void Triangle::createUniformBuffers()
{
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
	uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		VulkanHelper::createBuffer(physicalDevice, device, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);

		vkMapMemory(device, uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersMapped[i]);
	}
}

void Triangle::capsule_createUniformBuffers()
{
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	capsule_uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	capsule_uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
	capsule_uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		VulkanHelper::createBuffer(physicalDevice, device, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, capsule_uniformBuffers[i], capsule_uniformBuffersMemory[i]);

		vkMapMemory(device, capsule_uniformBuffersMemory[i], 0, bufferSize, 0, &capsule_uniformBuffersMapped[i]);
	}
}

/// <summary>
/// 创建描述符池
/// </summary>
void Triangle::createDescriptorPool()
{
	/*VkDescriptorPoolSize poolSize{};
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;
	poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

	if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}*/

	// 创建更大的描述符池，池不够大会失败
	//std::array<VkDescriptorPoolSize, 2> poolSizes{};
	//poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	//poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	//poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	//poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

	//VkDescriptorPoolCreateInfo poolInfo{};
	//poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	//poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	//poolInfo.pPoolSizes = poolSizes.data();
	//poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

	//if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
	//	throw std::runtime_error("failed to create descriptor pool!");
	//}



	// 创建新的描述符池
	VkDescriptorPoolSize pool_sizes[] =
	{
		{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
	};

	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
	pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
	pool_info.pPoolSizes = pool_sizes;
	VkResult err = vkCreateDescriptorPool(device, &pool_info, nullptr, &descriptorPool);
}

/// <summary>
/// 创建描述符集
/// </summary>
void Triangle::createDescriptorSets()
{
	std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, uniformDescriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	allocInfo.pSetLayouts = layouts.data();

	descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
	if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	capsule_descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
	if (vkAllocateDescriptorSets(device, &allocInfo, capsule_descriptorSets.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = uniformBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = descriptorSets[i];
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;

		vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);

		bufferInfo.buffer = capsule_uniformBuffers[i];
		descriptorWrite.dstSet = capsule_descriptorSets[i];
		vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
	}
}

/// <summary>
/// 创建命令缓冲区
/// </summary>
void Triangle::createCommandBuffers()
{
	commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

	if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}
}

/// <summary>
/// 记录命令缓冲区
/// </summary>
/// <param name="commandBuffer"></param>
/// <param name="imageIndex"></param>
void Triangle::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.pNext = NULL;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	beginInfo.pInheritanceInfo = NULL;

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("failed to begin recording command buffer!");
	}


	// 绘制原来的图像数据
	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderPass;
	renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = swapChainExtent;

	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
	clearValues[1].depthStencil = { 1.0f, 0 };

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	{
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)swapChainExtent.width;
		viewport.height = (float)swapChainExtent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = swapChainExtent;
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		//vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::vec3), &faceColor);
		//vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[currentFrame], 0, nullptr);
		//vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 1, 1, &m_texture.descriptorSet, 0, nullptr);

		/*model.Render(commandBuffer);*/
	}

	{
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, color_graphicsPipeline);

		vkCmdPushConstants(commandBuffer, color_pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstant), &pushConstant);
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, color_pipelineLayout, 0, 1, &capsule_descriptorSets[currentFrame], 0, nullptr);
		//vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 1, 1, &m_texture1.descriptorSet, 0, nullptr);

		model1.Render(commandBuffer);
		imguiRender(commandBuffer);
	}
	vkCmdEndRenderPass(commandBuffer);
	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to record command buffer!");
	}
}

/// <summary>
/// 渲染与演示
/// </summary>
void Triangle::drawFrame(Timestep timestep)
{
	//	等待上一帧完成
	//	从交换链获取图像
	//	记录将场景绘制到该图像上的命令缓冲区
	//	提交记录的命令缓冲区
	//	呈现交换链图像

	vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		recreateSwapChain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("failed to acquire swap chain image!");
	}


	cameraMove(timestep);

	updateUniformBuffer(currentFrame);
	capsule_updateUniformBuffer(currentFrame);

	vkResetFences(device, 1, &inFlightFences[currentFrame]);

	vkResetCommandBuffer(commandBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
	recordCommandBuffer(commandBuffers[currentFrame], imageIndex);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

	VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;

	presentInfo.pImageIndices = &imageIndex;

	result = vkQueuePresentKHR(presentQueue, &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
		framebufferResized = false;
		recreateSwapChain();
	}
	else if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to present swap chain image!");
	}

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

/// <summary>
/// 更新描述符数据
/// </summary>
/// <param name="currentImage"></param>
void Triangle::updateUniformBuffer(uint32_t currentImage)
{

	UniformBufferObject ubo{};
	ubo.model = m_LocalMatrix;// 模型矩阵，模型在 3d空间中的位置;
	ubo.view = glm::lookAt(mCamera_Position, mCamera_Position + mCamera_Target, mCamera_Up);// 相机的 position,targetPosition, yaw,roll,pitch
	ubo.proj = glm::perspective(glm::radians(m_aspect), swapChainExtent.width / (float)swapChainExtent.height, 0.1f, 100.0f);//设置相机 Fov, width,height,near,far;
	ubo.proj[1][1] *= -1;

	memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}

void Triangle::capsule_updateUniformBuffer(uint32_t currentImage)
{
	UniformBufferObject ubo{};
	ubo.model = capsule_m_LocalMatrix;// 模型矩阵，模型在 3d空间中的位置;
	ubo.view = glm::lookAt(mCamera_Position, mCamera_Position + mCamera_Target, mCamera_Up);// 相机的 position,targetPosition, yaw,roll,pitch
	ubo.proj = glm::perspective(glm::radians(m_aspect), swapChainExtent.width / (float)swapChainExtent.height, 0.1f, 100.0f);//设置相机 Fov, width,height,near,far;
	ubo.proj[1][1] *= -1;
	memcpy(capsule_uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}


/// <summary>
/// 创建同步对象
/// </summary>
void Triangle::createSyncObjects()
{
	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {

			throw std::runtime_error("failed to create synchronization objects for a frame!");
		}
	}
}

/// <summary>
/// 创建着色器模块
/// </summary>
/// <param name="code"></param>
/// <returns></returns>
VkShaderModule Triangle::createShaderModule(const std::vector<char>& code)
{
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module!");
	}

	return shaderModule;
}

/// <summary>
/// 创建ImageView 用于使用交换链上面VkImage
/// </summary>
void Triangle::createImageViews()
{
	// 调整大小来适应将来要创建的图像视图
	swapChainImageViews.resize(swapChainImages.size());
	for (size_t i = 0; i < swapChainImages.size(); i++)
	{
		swapChainImageViews[i] = VulkanHelper::createImageView(device, swapChainImages[i], swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
	}
}

/// <summary>
/// 选择物理设备
/// </summary>
void Triangle::pickPhysicalDevice()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
	if (deviceCount == 0) {
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
	// 实际应用中多显卡支持还要检查显卡的性能等信息。
	for (const auto& device : devices) {
		if (isDeviceSuitable(device)) {
			physicalDevice = device;
			msaaSamples = getMaxUsableSampleCount();
			break;
		}
	}

	if (physicalDevice == VK_NULL_HANDLE) {
		throw std::runtime_error("failed to find a suitable GPU!");
	}
}

/// <summary>
/// 创建逻辑设备
/// </summary>
void Triangle::createLogicalDevice()
{
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };
	// 图形队列
	float queuePriority = 1.0f;

	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();

	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
		throw std::runtime_error("failed to create logical device!");
	}

	vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
	vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);

}

/// <summary>
/// 检查验证层是否支持
/// </summary>
/// <returns></returns>
bool Triangle::checkValidationLayerSupport()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : validationLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			return false;
		}
	}

	return true;
}

/// <summary>
/// 选择合适的物理设备
/// </summary>
/// <param name="device"></param>
/// <returns></returns>
bool Triangle::isDeviceSuitable(VkPhysicalDevice device)
{
	QueueFamilyIndices indices = findQueueFamilies(device);

	bool extensionsSupported = checkDeviceExtensionSupport(device);

	bool swapChainAdequate = false;
	if (extensionsSupported) {
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

	return indices.isComplete() &&
		extensionsSupported &&
		swapChainAdequate &&
		supportedFeatures.samplerAnisotropy;
}

bool Triangle::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

/// <summary>
/// 查询交换链的支持信息
/// </summary>
/// <param name="device"></param>
/// <returns></returns>
SwapChainSupportDetails Triangle::querySwapChainSupport(VkPhysicalDevice device)
{
	SwapChainSupportDetails details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
	}
	return details;
}

/// <summary>
/// 选择交换链Format
/// </summary>
/// <param name="availableFormats"></param>
/// <returns></returns>
VkSurfaceFormatKHR Triangle::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	for (const auto& availableFormat : availableFormats)
	{
		if (availableFormat.format == VK_FORMAT_R8G8B8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return availableFormat;
		}
	}
	return availableFormats[0];
}

VkPresentModeKHR Triangle::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Triangle::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	}
	else {
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}
}

QueueFamilyIndices Triangle::findQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

		if (presentSupport) {
			indices.presentFamily = i;
		}

		if (indices.isComplete()) {
			break;
		}

		i++;
	}

	return indices;
}

/// <summary>
/// 主循环
/// </summary>
void Triangle::mainLoop()
{
	while (!glfwWindowShouldClose(window))
	{
		// 稳定60fps;
		float time = glfwGetTime();
		Timestep timestep = time - m_LastFrameTime;
		float accumulator = timestep;
		while (accumulator > 1.0 / 61.0)
		{
			m_LastFrameTime = time;
			accumulator -= 1.0 / 59.0;
			glfwPollEvents();
			drawFrame(timestep);
			if (accumulator < 0) accumulator = 0;
		}

	}

	vkDeviceWaitIdle(device);
}

/// <summary>
/// 项目清理
/// </summary>
void Triangle::cleanUp()
{
	clearImgui();
	//旧的交换链清理
	cleanupSwapChain();

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroyBuffer(device, uniformBuffers[i], nullptr);
		vkFreeMemory(device, uniformBuffersMemory[i], nullptr);

		vkDestroyBuffer(device, capsule_uniformBuffers[i], nullptr);
		vkFreeMemory(device, capsule_uniformBuffersMemory[i], nullptr);
	}

	m_texture.Destroy();
	m_texture1.Destroy();

	m_texture.Free();
	m_texture1.Free();

	vkDestroyDescriptorSetLayout(device, uniformDescriptorSetLayout, nullptr);
	vkDestroyDescriptorSetLayout(device, textureDescriptorSetLayout, nullptr);


	vkDestroyDescriptorPool(device, descriptorPool, nullptr);

	// 同步对象
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(device, inFlightFences[i], nullptr);
	}

	// 命令缓冲池，销毁时自动释放其中的命令缓冲区
	vkDestroyCommandPool(device, commandPool, nullptr);

	// 渲染管线
	vkDestroyPipeline(device, graphicsPipeline, nullptr);
	// 管线布局
	vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
	// 渲染通道
	vkDestroyRenderPass(device, renderPass, nullptr);

	//model.Destroy();

	model1.Destroy();

	//清理逻辑设备
	vkDestroyDevice(device, nullptr);
	//清理surface
	vkDestroySurfaceKHR(instance, surface, nullptr);
	//清理 VkInstance;
	vkDestroyInstance(instance, nullptr);

	glfwDestroyWindow(window);
	glfwTerminate();
}

//https://frguthmann.github.io/posts/vulkan_imgui/
/// <summary>
/// 初始化Imgui
/// </summary>
void Triangle::initImgui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls


	// Setup Dear ImGui style
	//ImGui::StyleColorsClassic();
	ImGuiStyle& style = ImGui::GetStyle();
	ImGui::StyleColorsDark();

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	//将引擎的相关部分暴露给 Dear ImGui
	// Setup Platform/Renderer bindings

	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

	ImGui_ImplGlfw_InitForVulkan(window, true);
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = instance;
	init_info.PhysicalDevice = physicalDevice;
	init_info.Device = device;
	init_info.QueueFamily = indices.graphicsFamily.value();
	init_info.Queue = graphicsQueue;
	init_info.PipelineCache = VK_NULL_HANDLE;
	init_info.DescriptorPool = descriptorPool;
	init_info.Allocator = nullptr;
	init_info.MinImageCount = MAX_FRAMES_IN_FLIGHT;
	init_info.ImageCount = swapChainImages.size();
	init_info.CheckVkResultFn = NULL;
	init_info.MSAASamples = msaaSamples;
	// 初始化 Vulkan;
	ImGui_ImplVulkan_Init(&init_info, renderPass);

	VkCommandBuffer command_buffer = VulkanHelper::beginSingleTimeCommands(device, commandPool);
	ImGui_ImplVulkan_CreateFontsTexture(command_buffer);
	VulkanHelper::endSingleTimeCommands(device, commandPool, graphicsQueue, command_buffer);
	vkDeviceWaitIdle(device);
	ImGui_ImplVulkan_DestroyFontUploadObjects();
}

/// <summary>
/// 清除 ImGui渲染相关代码
/// </summary>
void Triangle::clearImgui()
{
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void Triangle::imguiRender(VkCommandBuffer commandBuffer)
{
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	//static bool show_demo_window = false;
	//ImGui::ShowDemoWindow(&show_demo_window);

	float itemWidth = (ImGui::GetContentRegionAvail().x - (ImGui::GetFontSize() * 3.0f)) / 3.0f;
	bool showInspctor = true;
	ImGui::Begin("Inspactor", &showInspctor);
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Transform");
	ImGui::AlignTextToFramePadding();

	PorpertyTransform("Position", m_Position, itemWidth);

	ImGui::SameLine();
	PorpertyTransform("Rotation", m_Rotation, itemWidth);
	ImGui::SameLine();
	PorpertyTransform("Scale", m_Scale, itemWidth);

	ImGui::Columns(1);
	ImGui::Separator();

	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Camera");
	ImGui::AlignTextToFramePadding();

	PorpertyTransform("Position", mCamera_Position, itemWidth);

	ImGui::SameLine();
	PorpertyTransform("Traget", mCamera_Target, itemWidth);
	ImGui::SameLine();
	PorpertyTransform("Up", mCamera_Up, itemWidth);

	ImGui::ColorEdit4("Color", &colorEditor.r);

	ImGui::InputFloat3("Direction", &lightDirection[0]);

	pushConstant.color = glm::vec3(colorEditor.r, colorEditor.g, colorEditor.b);
	pushConstant.direction = glm::vec3(lightDirection[0], lightDirection[1], lightDirection[2]);

	ImGui::Columns(1);
	ImGui::Separator();
	ImGui::End();

	ImGui::Render();
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
	SetLocalMatrix();

}


bool Triangle::PorpertyTransform(const std::string& name, glm::vec3& vector, float width)
{
	const float labelIndetation = ImGui::GetFontSize();
	bool updated = false;

	auto& style = ImGui::GetStyle();

	const auto showFloat = [&](int axis, float* value)
	{
		const float label_float_spacing = ImGui::GetFontSize();
		const float step = 0.01f;
		const std::string format = "%.4f";

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(axis == 0 ? "X" : axis == 1 ? "Y"
			: "Z");
		ImGui::SameLine(label_float_spacing);
		ImVec2 pos_PostLabel = ImGui::GetCursorScreenPos();
		glm::vec2 posPostLabel;
		posPostLabel.x = pos_PostLabel.x;
		posPostLabel.y = pos_PostLabel.y;
		ImGui::PushItemWidth(width);
		ImGui::PushID(static_cast<int>(ImGui::GetCursorPosX() + ImGui::GetCursorPosY()));

		if (ImGui::InputFloat("##no_label", value, std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max(), format.c_str()))
			updated = true;

		ImGui::PopID();
		ImGui::PopItemWidth();

		static const ImU32 colourX = IM_COL32(168, 46, 2, 255);
		static const ImU32 colourY = IM_COL32(112, 162, 22, 255);
		static const ImU32 colourZ = IM_COL32(51, 122, 210, 255);

		const glm::vec2 size = glm::vec2(ImGui::GetFontSize() / 4.0f, ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y);
		posPostLabel = posPostLabel + glm::vec2(-1.0f, ImGui::GetStyle().FramePadding.y / 2.0f);
		ImRect axis_color_rect = ImRect(posPostLabel.x, posPostLabel.y, posPostLabel.x + size.x, posPostLabel.y + size.y);
		ImGui::GetWindowDrawList()->AddRectFilled(axis_color_rect.Min, axis_color_rect.Max, axis == 0 ? colourX : axis == 1 ? colourY
			: colourZ);
	};

	ImGui::BeginGroup();
	ImGui::Indent(labelIndetation);
	ImGui::TextUnformatted(name.c_str());
	ImGui::Unindent(labelIndetation);
	showFloat(0, &vector.x);
	showFloat(1, &vector.y);
	showFloat(2, &vector.z);
	ImGui::EndGroup();

	return updated;
}

void Triangle::SetLocalMatrix()
{
	float pitch = Min(m_Rotation.x, 89.9f);
	pitch = Max(pitch, -89.9f);
	m_LocalMatrix = glm::translate(glm::mat4(1.0), m_Position) * glm::toMat4(glm::quat(glm::radians(glm::vec3(pitch, m_Rotation.y, m_Rotation.z)))) * glm::scale(glm::mat4(1.0), m_Scale);
}

void Triangle::cameraMove(Timestep ts)
{
	std::cout << ts << std::endl;
	if (keys[GLFW_KEY_W])
	{
		mCamera_Position += (camerSpeed * ts) * mCamera_Target;
	}
	if (keys[GLFW_KEY_S])
	{
		mCamera_Position -= (camerSpeed * ts) * mCamera_Target;
	}
	if (keys[GLFW_KEY_A])
	{
		mCamera_Position -= glm::normalize(glm::cross(mCamera_Target, mCamera_Up)) * (camerSpeed * ts);
	}
	if (keys[GLFW_KEY_D])
	{
		mCamera_Position += glm::normalize(glm::cross(mCamera_Target, mCamera_Up)) * (camerSpeed * ts);
	}

	if (keys[GLFW_KEY_UP])
	{
		mCamera_Target.y += camerSpeed * ts;
	}

	if (keys[GLFW_KEY_DOWN])
	{
		mCamera_Target.y -= camerSpeed * ts;
	}

	if (keys[GLFW_KEY_LEFT])
	{
		mCamera_Target.x += camerSpeed * ts;
	}

	if (keys[GLFW_KEY_RIGHT])
	{
		mCamera_Target.x -= camerSpeed * ts;
	}
}

void Triangle::timeController()
{

}