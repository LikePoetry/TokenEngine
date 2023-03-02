// ����������

#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

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

	//��֤���Ƿ����
	bool checkValidationLayerSupport();

private:
	GLFWwindow* window;

	VkInstance instance;

private:
	const uint32_t WIDTH = 800;
	const uint32_t HEIGHT = 600;

	const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
	};

	const bool enableValidationLayers = true;		//������֤����Ϣ

};