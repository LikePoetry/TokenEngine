// 2023��3��2�� �����εĻ��ƽṹ

#include "Triangle.h"
#include <iostream>

Triangle::Triangle()
{

}

Triangle::~Triangle()
{

}

void Triangle::run()
{
	initWindow();
	initVulkan();
	mainLoop();
	cleanUp();
}

/// <summary>
/// ��ʼ��GLFW ����
/// </summary>
void Triangle::initWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
}

/// <summary>
/// ��ʼ��Vulkan
/// </summary>
void Triangle::initVulkan()
{
	createInstance();
}

/// <summary>
/// ����Vulkan ʵ��
/// </summary>
void Triangle::createInstance()
{
	//�����������ṩһЩ������Ϣ
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	//��������
	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	// �����չ֧��
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	createInfo.enabledExtensionCount = glfwExtensionCount;
	createInfo.ppEnabledExtensionNames = glfwExtensions;

	// ��Ҫ������Щ��չ��������ϸ˵��
	createInfo.enabledLayerCount = 0;

	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
		throw std::runtime_error("failed to create instance!");
	}
}

/// <summary>
/// ��ѭ��
/// </summary>
void Triangle::mainLoop()
{
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
	}
}

/// <summary>
/// ��Ŀ����
/// </summary>
void Triangle::cleanUp()
{
	//���� VkInstance;
	vkDestroyInstance(instance, nullptr);

	glfwDestroyWindow(window);
	glfwTerminate();
}
