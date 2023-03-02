// 2023��3��2�� �����εĻ��ƽṹ

#include "Triangle.h"

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
	 glfwDestroyWindow(window);
	 glfwTerminate();
 }
