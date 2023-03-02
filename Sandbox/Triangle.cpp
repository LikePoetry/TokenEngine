// 2023年3月2日 三角形的绘制结构

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
 /// 初始化GLFW 窗口
 /// </summary>
 void Triangle::initWindow()
 {
	 glfwInit();
	 glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	 glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	 window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
 }

 /// <summary>
 /// 初始化Vulkan
 /// </summary>
 void Triangle::initVulkan()
 {

 }

 /// <summary>
 /// 主循环
 /// </summary>
 void Triangle::mainLoop()
 {
	 while (!glfwWindowShouldClose(window))
	 {
		 glfwPollEvents();
	 }
 }

 /// <summary>
 /// 项目清理
 /// </summary>
 void Triangle::cleanUp()
 {
	 glfwDestroyWindow(window);
	 glfwTerminate();
 }
