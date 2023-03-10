// vulkan 学习进程文件，每次提交保证项目能够顺利运行;

//三角形的绘制

#include "Triangle.h"

#include <glm/glm.hpp>

int main()
{
	Triangle app;
	app.run();
	return 0;

	// 测试代码

	//glm::vec3 lightDirection = glm::normalize(glm::vec3(1.0, 3.0, -1.0));

	//glm::mat4 modelMatrix = glm::mat4(1);

	////发现方向
	//glm::vec3 inNormal = glm::normalize(glm::vec3(1.0, 1.0, 1.0));

	//glm::vec3 normalWorldSapce = glm::normalize(glm::mat3(modelMatrix) * inNormal);

	//float dotResult = glm::dot(normalWorldSapce, lightDirection);
	//float lightIntensity = Max(dotResult, 0);

	//glm::vec4 color = lightIntensity * glm::vec4(1.0, 0.0, 0.0, 1.0);

}