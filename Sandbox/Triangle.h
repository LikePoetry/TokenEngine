// ����������

#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

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

private:
	GLFWwindow* window;

	const uint32_t WIDTH = 800;
	const uint32_t HEIGHT = 600;

	VkInstance instance;
};